# Native bottom operand boundaries

M4x corrects an earlier documentation-based assumption. The supplied manual
says bottom propagates through ordinary expressions. The supplied beta's stable
numeric probes instead show ordinary binary/unary consumers treating bottom as
integer zero in the measured cases. Bottom itself remains distinct from zero.
This is not a claim that every malformed type/opcode has the same fallback.

Missing redirection still produces bottom and a warning. Thus a bare missing
destination is false as a trigger, but `1 + (Helper(999), ID)` returns 1, and
`(Parent, ID) = 0` returns true on a root. Arithmetic domain failures behave the
same at those operator boundaries: `Ln(-1)+2` and `(1%0)+2` return 2. Integer
coercion is visible in `5/(Ln(-1)+2)` returning 2 versus a float literal 2.0
giving 2.5. Unary not gives 1, comparisons and logical operators consume zero,
and bitwise OR with 4 gives 4. Both operands still evaluate ordinarily.

IfElse with a bottom condition selects its false branch after eager evaluation.
Cond with a bottom condition returns bottom without evaluating branches. The
native assignment case returns 34: both IfElse branches assign 11 then 17,
the selected result is 17, and the following Var read is 17. Selected bottom
branches remain bottom until a surrounding ordinary operator consumes them.

VarSet/VarAdd float and integer values consume bottom as zero, as do measured
VelSet/VelAdd components. A VarSet initialized to 73 becomes zero; VarAdd keeps
73. VelAdd preserves X=5 while adding Y to 8; VelSet replaces X with zero and Y
with 2. These controllers now support those boundaries. Other controller/entry/
index/function argument defaults and malformed-type behavior remain open; their
typed errors have not all been replaced by a universal zero fallback.

Bitwise NOT of a bottom operand is different: the native VM fails at runtime
with “malformed expression”. Go retains a fatal evaluation result and propagates
it through controller arguments and triggers with file/state/controller context.
It is not treated as an ordinary false trigger or recoverable zero. A headless
test checks that later effects do not run. Other fatal VM type/opcode cases
still need dedicated comparisons.

The stable programs identity-bottom, identity-missing, identity-bottom-operators
and identity-bottom-bitor pin 72 numeric values; identity-bottom-bitnot pins the
rejection. The remaining identity/family programs bring M4x to 240 numbers and
thirteen cases. Character tests compare all stable values. A pure evaluator
test replays the 24 operator values without DISPLAY, with hash-pinned CNS input.
The old JSONL expectation that `0&&(1/0)` is bottom is superseded by integer zero.

## Unresolved native exponentiation defect

`(1/0)**0` and `0**(1/0)` return the same non-mathematical integer converted to
float within a native run. In the larger operator program it was 73759720; an
isolated two-expression program returned 73735280 on both fresh repetitions.
Changing expression layout changes the result. This is consistent with stale
native data, but no memory-level cause is established yet.

Original programs and raw captures are retained as identity-bottom-power and
identity-bottom-power-layout with explicit unresolved status. They can be run
with reference_probe.py, but are not Go conformance goldens and are excluded
from the advertised stable observation count. Current Go returns deterministic
ordinary-zero exponent results; this remains an unexplained divergence, not
Verified behavior. Do not bake the observed integers into implementation or
silently mark these cases passed. Investigate the native VM and settle the
applicability of unstable error behavior before the final compatibility gate.
