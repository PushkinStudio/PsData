# Blueprints

The model is declared in C++, but it is not C++-only. The editor module (`PsDataEditor`) generates
Blueprint nodes from the same reflection the serializers use, so every field you declare is
readable — and, unless you say otherwise, writable — from a graph without a single `UFUNCTION`.

## Nodes per field

For every data class, the action menu has a **PsData › &lt;ClassName&gt;** category with:

| Node | For | Notes |
| --- | --- | --- |
| **Get &lt;Class&gt;::&lt;Field&gt;** | every field | Pure. The output pin has the field's real type. |
| **Set &lt;Class&gt;::&lt;Field&gt;** | fields that are not `Strict` or `ReadOnly` | Fires the same events a C++ assignment fires. |
| **Link** node | every `DLINK` | Pure; returns the resolved data (or array of data) with its declared class. |
| **Collection proxy** node | every `DARRAY` / `DMAP` | Returns a `PsDataBlueprintArrayProxy` / `PsDataBlueprintMapProxy` with `Get`, `IsValid`, `Bind` and `Unbind`. |

Fields marked `Deprecated` show their nodes as deprecated; `Hidden` fields still get nodes — `Hidden`
is about the wire, not about visibility.

The nodes store the class and the field name, so renaming a C++ field breaks the nodes that use
it; replace them after a rename. `Alias` does not help here: it renames the serialized key, not the
Blueprint-facing name.

## Listening

`UPsData` exposes the event system directly:

| Function | |
| --- | --- |
| **Bind** (`Type`, `Delegate`, `bNonDeferred`) | The `Type` pin is a dropdown of every event that can reach the target: the built-in ones, its own `Event` fields, and the `Bubbles` fields of every class below it in the tree. The delegate takes a `PsDataEvent`. |
| **Unbind** / **UnbindAll** | `UnbindAll(Object)` drops every binding that object made — call it from `EndPlay`. |
| **IsBound** | Whether anything listens to a type (optionally including ancestors, for bubbling events). |

On the event: **Get Type**, **Is Bubbles**, **Get Target** (the data the event started from),
**Stop Propagation**, **Stop Immediate Propagation**.

## Navigating the tree

**Get Parent**, **Get Root**, **Has Root**, **Get Data Key**, **Get Collection Key**,
**Get Full Data Key**, **Get Path From Root**, **In Collection** — the same calls as in C++
([`model.md`](model.md#the-tree)). **Get Hash** / **Get Hash Int32** give the content hash.

## Deterministic numbers in the details panel

`FPsDataFixedPoint` and `FPsDataBigInteger` get property customizations: they edit as exact decimal
text in details panels and DataTable rows, not as floats.

## Typical split

Programmers declare the model and own the code that mutates static data and protocol state;
designers and UI Blueprints read fields, follow links and bind to events. `DPROP_CONST` and
`ReadOnly` are how that split is enforced: the Set nodes simply do not exist for those fields.
