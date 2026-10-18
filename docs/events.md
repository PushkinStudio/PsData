# Events

Every change to the model can be observed. Listeners are ordinary delegates; the payload is a
`UPsDataEvent` that knows its type, the data it started from, and (optionally) the old and new value.

PsData is **game-thread only**. Its event dispatch and scope guard are process-wide statics with no
locking.

## Field events

A property with `Event` meta broadcasts `<Name>Changed` every time its value changes. The broadcast
is synchronous: listeners have run by the time the assignment returns. Assigning the value it
already holds broadcasts nothing.

```cpp
// in the class
DMETA(Event, EventStorage)
DPROP(int32, Health);

// anywhere
FPsDataBindCollection Binds;  // a member: unbinds everything when destroyed

Binds.Add(Character->Health.Bind(FPsDataDelegate::CreateLambda([Character](UPsDataEvent* Event) {
	const auto Storage = Character->Health.GetEventStorage(Event);  // needs EventStorage meta
	UE_LOG(LogTemp, Log, TEXT("Health %d -> %d"), Storage->GetPreviousValue(), Storage->GetValue());
})));
```

There are three ways to bind, all returning an `FPsDataBind`:

| Call | Listens to |
| --- | --- |
| `Data->Field.Bind(Delegate, Flags)` | that field's changed event |
| `Data->Bind(EventType, Delegate, Flags)` | any event type by name — `UFooData::GetHealthChangedEventName()`, `UPsDataEvent::Changed`, a custom `Event = Name` |
| `Data->Collection->Bind(UPsDataEvent::Added / Removed, Delegate, Flags)` | elements entering or leaving that one collection |

Delegates are `FPsDataDelegate` (any C++ callable) or `FPsDataDynamicDelegate` (a `UFUNCTION`,
what Blueprints use). Keep the returned `FPsDataBind` in an `FPsDataBindCollection` and call
`Unbind()` on it in `EndPlay`/`Deinitialize`; `Data->UnbindAll(Object)` removes every listener a
given object owns.

### Fields without `Event` meta

A field without `Event` is silent — no event object is built, nothing is dispatched. A listener
can opt in with `EDataBindFlags::IgnoreFieldMeta`:

```cpp
Binds.Add(Character->Kills.Bind(Delegate, EDataBindFlags::IgnoreFieldMeta));
```

Once at least one such listener exists, the field broadcasts, and **every** listener of that event
hears it, not only the `IgnoreFieldMeta` one (`PsData.Events.Field` pins this down).

## Bubbling

With `Bubbles`, the event travels from the data that changed up through every ancestor. A listener
anywhere above sees it; `Event->GetTarget()` is the data where it started.

```cpp
// UFooPointData
DMETA(Event, Bubbles)
DPROP(int32, X);

// one listener for every character's position on the whole battle
Binds.Add(Battle->Bind(UFooPointData::GetXChangedEventName(), FPsDataDelegate::CreateLambda([](UPsDataEvent* Event) {
	const auto Point = Cast<UFooPointData>(Event->GetTarget());
	const auto Character = Cast<UFooCharacterData>(Point->GetParent());
})));
```

`Event->StopPropagation()` stops the event at the current level after its listeners run;
`StopImmediatePropagation()` also skips the remaining listeners on the current level.

## Built-in events

| Type | On | Bubbles | When |
| --- | --- | --- | --- |
| `UPsDataEvent::Added` | the child, then its parent | yes | the child was put into a collection or a data property |
| `UPsDataEvent::Removed` | the child, then its parent | yes | the child was taken out |
| `UPsDataEvent::AddedToParent` / `RemovedFromParent` | the child | no | same moments, child side only |
| `UPsDataEvent::AddedToRoot` / `RemovedFromRoot` | the child and every descendant | no | the subtree became reachable / unreachable from a `UPsDataRoot` |
| `UPsDataEvent::NameChanged` | the child | no | its key or collection changed |
| `UPsDataEvent::Changed` | the data whose field changed | no | **deferred**, see below |

Binding `Added`/`Removed` through a collection proxy (`Battle->Characters->Bind(...)`) filters to
that collection; binding on the owner (`Battle->Bind(UPsDataEvent::Added, ...)`) hears every
element added anywhere below it, since the event bubbles.

## The deferred `Changed` event

`UPsDataEvent::Changed` answers "did anything about this object change?" without a listener per
field. It does not fire on each assignment: the first change schedules a game-thread task, and the
task broadcasts once, however many fields changed in between. It is per object — a change in a child
does not mark its parent — and it does not bubble.

```cpp
Binds.Add(Settings->Bind(UPsDataEvent::Changed, FPsDataDelegate::CreateUObject(this, &UFooSettingsSubsystem::RequestSave)));

Settings->MasterVolume = 0.5f;
Settings->Language = TEXT("en");
// RequestSave runs once, after this code returns, on the game thread
```

This is the event to hang persistence, dirty flags and UI refreshes on.

## Holding events back

Inside a `DEFERRED_EVENT_PROCESSING()` scope, listeners are held until the outermost scope ends,
then run in order. Listeners bound with `EDataBindFlags::NonDeferred` still run inline.

```cpp
{
	DEFERRED_EVENT_PROCESSING();

	Character->Health = 0;
	Character->Deaths += 1;
	Battle->Characters->Remove(Character->GetDataKey());
	// nothing bound without NonDeferred has run yet: no listener sees a half-applied state
}
// all of it is delivered here
```

Every single assignment already runs inside such a scope, so a listener never observes a property
mid-write. Link caches use `NonDeferred` internally, which is why a link is correct even inside a
held scope.

## Promises

`FPsDataSimplePromise` and `TPsDataPromise<T>` are one-shot readiness signals: `Bind` before
`Resolve` runs on `Resolve`; `Bind` after it runs immediately. The plugin's own is
`FDataDelegates::PostDataModuleInitPromise`, resolved when PsData's reflection is compiled — the
moment from which a model can be built (see [`model.md`](model.md#reflection-lifecycle)).

```cpp
FPsDataSimplePromise ModelReady;  // a member of whatever builds the model

// elsewhere, at any time
ModelReady.Bind(FSimpleDelegate::CreateUObject(this, &UFooHud::Subscribe));
```
