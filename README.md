# Emertxe Red-Black Tree

A robust, menu-driven implementation of a self-balancing Red-Black Tree written in C. Built as part of the Emertxe Training Project, it strictly upholds all four Red-Black properties to guarantee **O(log n)** time complexity for insertion, deletion, and search across all cases.

> **Honest note on AI involvement:** Portions of this documentation — including this README, the CONTRIBUTING guide, SECURITY policy, and GitHub templates — were drafted with the assistance of Claude (Anthropic). The core C implementation, algorithmic design, and debugging were done by the project authors. The documentation was reviewed, corrected, and approved by them. AI was used as a writing assistant, not as the engineer.

---

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Red-Black Tree Properties](#red-black-tree-properties)
- [Installation & Build](#installation--build)
- [Usage](#usage)
- [Testing](#testing)
- [Development Challenges](#development-challenges)
- [Performance](#performance)
- [Contributing](#contributing)
- [License](#license)

---

## Features

The program exposes an interactive terminal menu with 9 operations:

| # | Operation    | Description                                                   |
|---|--------------|---------------------------------------------------------------|
| 1 | Insert       | BST insert + automatic RB property restoration (recolor/rotate) |
| 2 | Delete       | BST delete with transplant logic + Double Black fix-up        |
| 3 | Search       | Recursive key lookup                                          |
| 4 | Display      | In-order traversal with color-coded terminal output           |
| 5 | Find MIN     | Leftmost node traversal                                       |
| 6 | Find MAX     | Rightmost node traversal                                      |
| 7 | Delete MIN   | Find + safely delete the minimum element                      |
| 8 | Delete MAX   | Find + safely delete the maximum element                      |
| 9 | Exit         | Frees all heap memory before quitting                         |

---

## Architecture

The codebase is deliberately modularized to enforce separation of concerns and make individual components independently testable.

```
red-black-tree/
├── tree.h          — Structs (node_t), enums (color_t, Status), all prototypes
├── main.c          — Interactive menu, entry point
├── color.h         — ANSI escape code macros for terminal color theming
├── insertion.c     — BST insert + balance_t (Red-Red violation fix via uncle check)
├── delete.c        — BST delete + transplant + delete_fix_up (Double Black resolution)
├── rotation.c      — left_rotation, right_rotation, and LL/RR/LR/RL router
├── search.c        — Recursive search_t
├── find_min.c      — Leftmost node traversal
├── find_max.c      — Rightmost node traversal
├── display.c       — In-order traversal with ANSI color output
├── tree_utils.c    — recolor(), free_tree(), get_color(), set_color() safe wrappers
├── test.c          — Comprehensive unit test suite (9 operation categories)
└── makefile        — Targets: all, test, clean
```

### Data Structures

```c
typedef enum { RED, BLACK } color_t;

typedef struct node {
    int      data;
    color_t  color;
    struct node *left;
    struct node *right;
    struct node *parent;
} node_t;
```

---

## Red-Black Tree Properties

This implementation enforces all four canonical properties:

1. Every node is either RED or BLACK.
2. The root is always BLACK.
3. Every RED node must have two BLACK children (no two consecutive RED nodes on any path).
4. All paths from any node to its descendant NIL leaves contain the same number of BLACK nodes (uniform black-height).

NIL leaves are represented as `NULL` pointers and are treated as BLACK via the `get_color()` safety wrapper.

---

## Installation & Build

### Prerequisites

- GCC (or any C99-compliant compiler)
- GNU Make
- A POSIX-compatible terminal (for ANSI color output)

### Build

```bash
# Clone the repository
git clone https://github.com/your-username/red-black-tree.git
cd red-black-tree

# Compile the main interactive program
make all

# Run it
./rbt
```

### Run Tests

```bash
make test
./test_rbt
```

### Clean Build Artifacts

```bash
make clean
```

The `makefile` compiles with `-Wall -Wextra -g` flags. All object files land in the project root.

---

## Usage

After running `./rbt`, you will be presented with a color-coded terminal menu:

```
══════════════════════════════════════
      RED-BLACK TREE  —  MAIN MENU
══════════════════════════════════════
  1. Insert
  2. Delete
  3. Search
  4. Display
  ...
══════════════════════════════════════
Enter your choice:
```

A sample display output after inserting `10, 20, 30, 15`:

```
(10)--(BLACK->1)  (15)--(RED->0)  (20)--(BLACK->1)  (30)--(RED->0)
```

RED nodes are printed in red terminal text; BLACK nodes in gray. Colors reset automatically after each node so the prompt is never affected.

---

## Testing

The `test.c` suite covers all 9 menu operations as unit tests with `assert()` checks.

Run with:

```bash
make test && ./test_rbt
```

Expected output:

```
==================================================
   RED-BLACK TREE COMPREHENSIVE UNIT TESTS
==================================================

Testing Empty Tree Edge Cases...
  [PASS] Empty tree handled safely.

Testing Case 1: Insertions & Balancing...
  [PASS] All 4 rotation cases correct.

Testing Case 3 & 4: Search & Display...
  [PASS] Search logic verified.

Testing Case 5 & 6: Find MIN / MAX...
  [PASS] Minimum is 1, Maximum is 70.

Testing Case 7 & 8: Delete MIN / MAX...
  [PASS] Min and Max successfully deleted and updated.

Testing Case 2: Complex Internal Deletions...
  [PASS] Internal node and Leaf deletions succeeded.

Testing Case 9: Free Memory...
  [PASS] Tree memory successfully freed.

==================================================
 [SUCCESS] ALL 9 OPERATIONS PASSED FLAWLESSLY!
==================================================
```

### Test Coverage Summary

| Category                        | Result    |
|---------------------------------|-----------|
| Empty tree edge cases           | 100% PASS |
| RR rotation (Left Rotation)     | 100% PASS |
| LL rotation (Right Rotation)    | 100% PASS |
| LR double rotation              | 100% PASS |
| RL double rotation              | 100% PASS |
| 12-node stress insertion        | 100% PASS |
| Min/Max delete & update         | 100% PASS |
| Internal node deletion (2 children) | 100% PASS |
| Memory deallocation (Valgrind)  | 0 bytes leaked |

---

## Development Challenges

These are real problems encountered during implementation — not theoretical caveats.

### Handling NIL Leaves (Segmentation Faults)

NULL pointers conceptually represent the black "NIL" sentinel leaves of a Red-Black Tree. Accessing `.color` directly on a NULL pointer caused segmentation faults in early builds. The fix was abstracting all color access into `get_color(node_t *)` and `set_color(node_t *, color_t)` wrappers that check for NULL before dereferencing. Every color read in `delete_fix_up` goes through these wrappers.

### The Double Black Deletion Problem

Standard BST deletion is not sufficient for a Red-Black Tree. When a BLACK node with a BLACK replacement child is removed, the black-height property breaks along that path — a condition called "Double Black." Resolving it required implementing `delete_fix_up` with 4 distinct sub-cases (and their mirror images for right-child scenarios), determined by the sibling's color and its children's colors. Because the replacement node `x` is frequently `NULL` itself, a separate `x_parent` pointer had to be tracked throughout the entire deletion routine to avoid NULL dereferences inside the fix-up loop.

### Short-Circuit Logic Bugs

An early crash was traced to the order of conditions in an `if` statement. Writing `uncle->color == BLACK || uncle == NULL` dereferences `uncle` before checking whether it is NULL. The corrected form — `uncle == NULL || uncle->color == BLACK` — takes advantage of C's guaranteed left-to-right short-circuit evaluation. This is an easy mistake to make and a good example of why the order of operands in boolean expressions matters in C.

### Empty Tree Edge Cases

Read-only traversal functions (`find_min`, `find_max`) initially crashed on empty trees because the NULL guard was placed after the first pointer dereference instead of before it. The fix was adding an explicit `if (root == NULL) return NULL;` at the very top of each function, before any member access.

### Accidental Data Loss via Switch-Case Fallthrough

A missing `break` statement between `case 8` (Delete Max) and `case 9` (Exit) caused `free_tree` to be called immediately after a max deletion, wiping the entire tree from memory without any user intent. The bug was subtle because the program did not crash — it simply started behaving as if the tree were always empty after a Delete Max. This reinforced two habits: always write the `break` before writing the case body, and always run the interactive menu through every path during integration testing, not just the unit tests.

---

## Performance

All core operations maintain **O(log n)** worst-case time complexity due to the enforced height bound of the Red-Black Tree (at most `2 * log₂(n+1)`).

| Operation    | Average | Worst Case |
|--------------|---------|------------|
| Insert       | O(log n) | O(log n)  |
| Delete       | O(log n) | O(log n)  |
| Search       | O(log n) | O(log n)  |
| Find MIN/MAX | O(log n) | O(log n)  |
| Display      | O(n)     | O(n)      |
| Free Tree    | O(n)     | O(n)      |

Space complexity is **O(n)** for the tree itself. Rotation and recoloring operations are O(1) per step.

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines on submitting patches, adding test cases, and running the build system.

---

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for the full text.