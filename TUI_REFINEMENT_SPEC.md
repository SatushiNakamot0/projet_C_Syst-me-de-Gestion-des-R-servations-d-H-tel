# TUI Refinement Specification - Reference Quality Terminal UI

## 🔶 STEP 1 — UI/UX PHILOSOPHY

### Why Terminal UIs Fail Most of the Time

**1. Cognitive Overload**

- Information density without hierarchy
- No visual grouping or whitespace
- Everything competes for attention equally
- Users must parse everything to find what they need

**2. Poor Visual Hierarchy**

- No distinction between primary and secondary information
- All text rendered at same intensity
- No clear entry points or focal areas
- Actions buried in walls of text

**3. Lack of Feedback**

- Silent failures with no indication
- No loading states or progress indicators
- Actions feel unresponsive
- No confirmation of state changes

**4. Inconsistent Navigation**

- Different keys do different things in different contexts
- No discoverability of available actions
- Escape logic inconsistent (sometimes cancels, sometimes goes back)
- No visual indication of current mode or focus

**5. Accessibility Issues**

- Assumes perfect color vision
- No consideration for low-contrast displays
- Dense text causes eye strain
- No keyboard-only alternatives

### How This Project Avoids Those Failures

**1. Clear Visual Hierarchy**

- Header (context) → Sidebar (navigation) → Content (primary) → Footer (actions)
- Color semantics: Primary (white), Secondary (dim), Accent (cyan), Status (green/yellow/red)
- Typography: Titles use accent color, body uses primary, metadata uses dim
- Spacing: Consistent margins create breathing room

**2. Focus Management**

- Always clear what has focus (highlighted selection)
- Visual feedback on every interaction
- Predictable navigation patterns
- Mode indicators (normal/edit/search)

**3. Component-Based Architecture**

- Reusable components with consistent behavior
- Each component handles its own focus and input
- Components communicate through well-defined interfaces
- Reduces cognitive load through familiarity

**4. Progressive Disclosure**

- Dashboard shows summary, details on demand
- Tables show essential info, expand for details
- Forms reveal fields as needed
- Help contextual to current screen

**5. Error Prevention & Recovery**

- Validation before submission
- Clear error messages with actionable guidance
- Undo/confirmation for destructive actions
- Graceful degradation on errors

---

## 🔶 STEP 2 — VISUAL LANGUAGE DEFINITION

### Color Semantics (Strict Rules)

**Primary Colors**

- `COLOR_PRIMARY_TEXT`: White (#FFFFFF) - Main content, readable text
- `COLOR_PRIMARY_BG`: Default terminal background - Content areas
- **Usage**: Body text, table data, form labels

**Secondary Colors**

- `COLOR_SECONDARY_TEXT`: Gray (#888888) - Metadata, timestamps, hints
- `COLOR_SECONDARY_BG`: Black (#000000) - Sidebar, inactive areas
- **Usage**: Footer text, disabled items, secondary information

**Accent Colors**

- `COLOR_ACCENT`: Cyan (#00FFFF) - Titles, highlights, important info
- **Usage**: Screen titles, KPI labels, important numbers

**Status Colors**

- `COLOR_SUCCESS`: Green (#00FF00) - Successful operations, positive metrics
- `COLOR_WARNING`: Yellow (#FFFF00) - Warnings, attention needed
- `COLOR_ERROR`: Red (#FF0000) - Errors, critical issues
- `COLOR_INFO`: Blue (#0088FF) - Informational messages, links
- **Usage**: Status messages, progress indicators, alerts

**Interactive States**

- `COLOR_FOCUS`: Blue background (#000088) with white text - Current selection
- `COLOR_HOVER`: Slightly brighter blue (#0000AA) - Preview of selection
- `COLOR_DISABLED`: Gray (#444444) - Unavailable actions
- **Usage**: Menu selection, table row selection, button states

**Semantic Rules**

- NEVER use colors arbitrarily
- Status colors ONLY for status/feedback
- Accent color ONLY for emphasis, never for body text
- Disabled state MUST be visually distinct
- Focus MUST be immediately obvious

### Spacing Rules

**Unit System**: 1 unit = 1 character (monospace assumption)

**Margins**

- Outer margin: 0 (terminal edge)
- Inner margin: 1 character (between regions)
- Component padding: 1 character (inside components)
- Text padding: 1 space (around text in buttons/labels)

**Vertical Rhythm**

- Header: 1 line
- Section spacing: 2 lines between major sections
- Item spacing: 1 line between list items
- Form field spacing: 1 line between fields
- Footer: 1 line

**Horizontal Rhythm**

- Sidebar width: 20 characters (18 on small terminals)
- Content margin: 1 character from sidebar
- Column spacing in tables: 1 character
- Button spacing: 2 characters

### Alignment Rules

**Text Alignment**

- Headers: Left-aligned (readability)
- Numbers: Right-aligned (easier comparison)
- Labels: Left-aligned with consistent width
- Values: Left-aligned after label
- Center: ONLY for titles and dialogs

**Component Alignment**

- Sidebar: Left edge
- Content: Left-aligned with sidebar margin
- Footer: Left-aligned, shortcuts right-aligned
- Tables: Columns aligned by type (text left, numbers right)

### Typography Rules

**Font Assumptions**

- Monospace font (terminal requirement)
- Fixed width characters
- No font size variation (use color/weight instead)

**Text Weight (Simulated)**

- Normal: Standard color
- Bold: Bright color (A_BOLD attribute)
- Dim: Dim color (A_DIM attribute)

**Text Styles**

- Titles: Accent color + bold
- Headers: Primary color + bold
- Body: Primary color
- Metadata: Secondary color + dim
- Code/IDs: Accent color

---

## 🔶 STEP 3 — GLOBAL LAYOUT REFINEMENT

### Layout Mathematics

**Terminal Dimensions**: `rows × cols`

**Header Region**

```
y: 0
x: 0
height: 1
width: cols
```

- Fixed height, full width
- Contains: App title, date/time
- No wasted space

**Sidebar Region**

```
y: 1 (below header)
x: 0
height: rows - 3 (header + footer + separator)
width: 20 (18 if cols < 100)
```

- Fixed width, dynamic height
- Contains: Navigation menu
- Proportional: ~20% of width on large terminals

**Content Region**

```
y: 1 (same as sidebar)
x: sidebar_width + 1 (margin)
height: rows - 3 (same as sidebar)
width: cols - sidebar_width - 1 (margin)
```

- Dynamic width, dynamic height
- Contains: Main content (tables, forms, etc.)
- Proportional: ~80% of width on large terminals

**Footer Region**

```
y: rows - 2 (above bottom)
x: 0
height: 1
width: cols
```

- Fixed height, full width
- Contains: Shortcuts, status messages
- No wasted space

**Status Message Region** (Overlay)

```
y: rows - 3 (above footer)
x: 2
height: 1
width: cols - 4
```

- Overlays content when active
- Auto-dismisses after timeout

### Graceful Degradation

**Small Terminal (< 80 cols)**

- Sidebar: 16 characters
- Content: Remaining space
- Hide non-essential info
- Stack elements vertically

**Short Terminal (< 24 rows)**

- Reduce section spacing
- Compact tables (fewer visible rows)
- Scroll indicators more prominent

**Minimum Terminal (80×24)**

- All regions visible
- Essential functionality preserved
- Scroll where needed

---

## 🔶 STEP 4 — COMPONENT SYSTEM

### Component Architecture

Each component is a self-contained unit with:

- **State**: Internal data
- **Draw function**: Renders component
- **Input handler**: Processes keyboard input
- **Focus behavior**: Manages focus state
- **Visual states**: Normal, focused, disabled, error

### Component Types

**1. Window Container**

- Purpose: Group related content
- Features: Border, title, scrollable content
- States: Normal, focused

**2. Table**

- Purpose: Display tabular data
- Features: Headers, rows, selection, scrolling, sorting
- States: Normal, row selected, column sorted

**3. Form**

- Purpose: Input collection
- Features: Fields, labels, validation, focus management
- States: Normal, field focused, field error, submitting

**4. Input Field**

- Purpose: Single value input
- Features: Cursor, validation, placeholder, mask
- States: Normal, focused, error, disabled

**5. Button**

- Purpose: Action trigger
- Features: Label, shortcut key, confirmation
- States: Normal, focused, pressed, disabled

**6. Status Message**

- Purpose: Feedback display
- Features: Type (success/warning/error/info), auto-dismiss, actions
- States: Showing, dismissing, dismissed

**7. Modal Dialog**

- Purpose: Interrupting interaction
- Features: Overlay, title, message, buttons, focus trap
- States: Showing, button focused, closing

**8. Progress Bar**

- Purpose: Progress indication
- Features: Percentage, label, animation
- States: Indeterminate, determinate, complete

---

## 🔶 STEP 5 — SCREEN-BY-SCREEN PERFECTION

### Dashboard

**Layout**

```
┌─────────────────────────────────────────────────────────────┐
│ Header: Title + Date/Time                                    │
├──────┬──────────────────────────────────────────────────────┤
│      │                                                       │
│ Nav  │  DASHBOARD                                            │
│      │                                                       │
│      │  ┌──────────────┐  ┌──────────────┐                 │
│      │  │ Total Clients│  │ Total Rooms │                 │
│      │  │     42       │  │     25       │                 │
│      │  │ [C] Active   │  │ [R] 68.0%   │                 │
│      │  └──────────────┘  └──────────────┘                 │
│      │                                                       │
│      │  ┌──────────────┐  ┌──────────────┐                 │
│      │  │ Reservations │  │ Total Revenue│                 │
│      │  │     18       │  │ 12,450.00€  │                 │
│      │  │ [B] Active   │  │ [$] Invoices │                 │
│      │  └──────────────┘  └──────────────┘                 │
│      │                                                       │
│      │  Overall Occupancy Rate:                             │
│      │  [████████████████░░░░] 68.0%                        │
│      │                                                       │
└──────┴──────────────────────────────────────────────────────┘
```

**Improvements**:

- KPI cards with consistent spacing
- Progress bar with visual fill
- Icons for quick recognition
- Numbers right-aligned for comparison
- Color coding: Green for good, yellow for attention

### Clients List

**Layout**

```
┌─────────────────────────────────────────────────────────────┐
│ Header                                                       │
├──────┬──────────────────────────────────────────────────────┤
│      │  CLIENTS MANAGEMENT                    [Search: ___] │
│ Nav  │                                                       │
│      │  ID  │ Nom      │ Prenom  │ Email        │ Tel      │
│      │ ─────┼──────────┼─────────┼──────────────┼──────────│
│      │  1   │ Dupont   │ Jean    │ jean@e.com  │ 06123456 │
│      │  2   │ Martin   │ Marie   │ marie@e.com │ 06234567 │
│      │ >3<  │ Bernard  │ Pierre  │ pierre@e.co │ 06345678 │ ← Selected
│      │  4   │ Dubois   │ Sophie  │ sophie@e.co │ 06456789 │
│      │                                                       │
│      │  [A]dd  [E]dit  [D]elete  [S]earch  [ESC]Back        │
└──────┴──────────────────────────────────────────────────────┘
```

**Improvements**:

- Search bar always visible
- Selected row clearly highlighted
- Column headers fixed on scroll
- Scroll indicators (↑↓) when content overflows
- Inline actions visible

### Billing List

**Layout**

```
┌─────────────────────────────────────────────────────────────┐
│ Header                                                       │
├──────┬──────────────────────────────────────────────────────┤
│      │  BILLING & INVOICES                                   │
│ Nav  │                                                       │
│      │  ID │ Client │ Nuits │ Prix/Nuit │ Total             │
│      │ ────┼────────┼───────┼───────────┼──────────────────│
│      │  1  │   5    │   3   │ 120.00€   │   360.00€        │
│      │  2  │   8    │   7   │ 150.00€   │ 1,050.00€        │
│      │ >3< │  12    │   2   │ 200.00€   │   400.00€        │
│      │                                                       │
│      │  Total Revenue: 12,450.00€                           │
│      │                                                       │
│      │  [C]reate  [V]iew  [E]xport  [ESC]Back               │
└──────┴──────────────────────────────────────────────────────┘
```

**Improvements**:

- Totals prominently displayed
- Currency formatting consistent
- Numbers right-aligned
- Summary row at bottom
- Export functionality visible

---

## 🔶 STEP 6 — INPUT & NAVIGATION UX

### Global Shortcuts (Always Available)

| Key         | Action                   | Context           |
| ----------- | ------------------------ | ----------------- |
| `Q`         | Quit (with confirmation) | Everywhere        |
| `F1`        | Help                     | Everywhere        |
| `ESC`       | Back/Cancel              | Context-dependent |
| `Tab`       | Next field/focus         | Forms             |
| `Shift+Tab` | Previous field           | Forms             |

### Local Shortcuts (Screen-Specific)

**Dashboard**

- `1-5`: Jump to section (if applicable)

**Lists (Clients/Rooms/Reservations/Billing)**

- `A`: Add new
- `E`: Edit selected
- `D`: Delete selected
- `S`: Search
- `/`: Quick search (focus search bar)
- `↑↓`: Navigate list
- `PgUp/PgDn`: Page navigation
- `Home/End`: First/Last item

**Forms**

- `Enter`: Submit
- `ESC`: Cancel
- `↑↓`: Previous/Next field
- `Ctrl+U`: Clear field
- `Ctrl+W`: Delete word

### Modal Key Behavior

**In Dialog**

- `Tab`: Cycle buttons
- `Enter`: Confirm (default button)
- `ESC`: Cancel
- `Y/N`: Yes/No (if applicable)

### Escape Logic Consistency

**Rule**: ESC always means "go back one level"

- In form → Cancel form, return to list
- In list → Return to dashboard
- In dialog → Close dialog, return to previous state
- At dashboard → Show quit confirmation

### Error Recovery Paths

1. **Validation Error**: Stay in form, highlight error, show message
2. **Save Error**: Show error dialog, allow retry or cancel
3. **Network Error**: Show error, allow retry
4. **Unexpected Error**: Log error, show user-friendly message, allow continue

---

## 🔶 STEP 7 — PERFORMANCE & REDRAW STRATEGY

### Dirty Region Redraws

**Concept**: Only redraw what changed, not entire screen

**Implementation**:

```c
typedef struct {
    bool header_dirty;
    bool sidebar_dirty;
    bool content_dirty;
    bool footer_dirty;
    bool status_dirty;
    int dirty_regions[4]; // y, x, h, w for each region
} DirtyFlags;
```

**Rules**:

- Track which regions changed
- Redraw only dirty regions
- Clear dirty flags after redraw
- Full redraw only on state change or resize

### Partial Window Refresh

**ncurses Functions**:

- `touchwin()`: Mark window as needing refresh
- `wnoutrefresh()`: Prepare refresh without updating screen
- `doupdate()`: Apply all prepared refreshes at once

**Strategy**:

1. Prepare all window updates
2. Call `wnoutrefresh()` for each window
3. Single `doupdate()` at end
4. Reduces flicker and improves performance

### Flicker Avoidance

**Techniques**:

1. Double buffering: Prepare off-screen, then swap
2. Batch updates: Group related draws
3. Use `curs_set(0)` to hide cursor during redraw
4. Minimize `clear()` calls
5. Use `wrefresh()` instead of `refresh()` when possible

### Resize Handling (SIGWINCH)

**Process**:

1. Signal handler sets resize flag
2. Main loop detects flag
3. Recalculate layout
4. Mark all regions dirty
5. Full redraw
6. Clear resize flag

**Optimization**: Debounce rapid resizes (wait 100ms before processing)

---

## 🔶 STEP 8 — ACCESSIBILITY & COMFORT

### Color-Blind Safe Palette

**Protanopia/Deuteranopia (Red-Green)**

- Use blue/yellow instead of red/green
- Add symbols/icons in addition to color
- Ensure contrast sufficient without color

**Tritanopia (Blue-Yellow)**

- Use red/green for status (less common)
- Ensure text readable without color

**Implementation**:

- Status: Color + Symbol (✓ ✗ ⚠ ℹ)
- Selection: Color + Highlight character (> <)
- Errors: Color + "ERROR:" prefix

### Low-Contrast Environments

**Adjustments**:

- Increase contrast for borders
- Use bold for important text
- Ensure 4.5:1 contrast ratio minimum
- Test on actual low-contrast displays

### Long-Session Fatigue

**Considerations**:

- Soft colors (not bright white on black)
- Adequate spacing (reduces eye strain)
- Clear focus indicators (reduces searching)
- Consistent layout (reduces cognitive load)
- Status messages auto-dismiss (reduces clutter)

### Typography

**Monospace Assumptions**:

- Fixed-width characters
- Consistent spacing
- Aligned columns
- No kerning issues

**Readability**:

- Line length: Max 80 characters (comfortable reading)
- Paragraph spacing: 1 blank line
- List spacing: 1 line between items
- Code/IDs: Monospace, accent color

---

## 🔶 STEP 9 — CODE DELIVERY

### File Structure (Respecting Existing)

```
ui/
├── ui.h / ui.c              # Main entry point (existing)
├── ui_state.h               # State definitions (existing)
├── ui_theme.h / ui_theme.c  # Theme system (enhanced)
├── ui_layout.h / ui_layout.c # Layout engine (enhanced)
├── ui_draw.h / ui_draw.c    # Drawing functions (enhanced)
├── ui_input.h / ui_input.c  # Input handling (enhanced)
├── ui_utils.h / ui_utils.c  # Utilities (enhanced)
├── ui_components.h / ui_components.c # NEW: Component system
└── ui_redraw.h / ui_redraw.c # NEW: Redraw optimization
```

### Code Quality Standards

**Documentation**:

- Every function has header comment explaining purpose
- Complex algorithms have inline comments
- Design decisions documented
- No "what" comments (code is self-documenting)
- Only "why" comments (explain reasoning)

**Ownership**:

- Each module owns its responsibility
- No cross-dependencies
- Clear interfaces between modules
- Minimal globals (only when necessary)

**Error Handling**:

- All functions return error codes or bool
- Check all return values
- Graceful degradation on errors
- User-friendly error messages

---

## 🔶 STEP 10 — QUALITY CONTROL

### UX Checklist

**Visual Hierarchy**

- [ ] Clear primary/secondary information distinction
- [ ] Consistent color usage
- [ ] Adequate whitespace
- [ ] Focus always visible

**Navigation**

- [ ] Predictable keyboard shortcuts
- [ ] Consistent ESC behavior
- [ ] Clear current location
- [ ] Easy to discover actions

**Feedback**

- [ ] Every action has visual feedback
- [ ] Status messages clear and actionable
- [ ] Loading states visible
- [ ] Errors explained clearly

**Performance**

- [ ] No noticeable lag
- [ ] Smooth scrolling
- [ ] Fast screen transitions
- [ ] Responsive to input

### Common TUI Anti-Patterns Avoided

1. ❌ **Full screen clear on every update** → ✅ Dirty region redraws
2. ❌ **Arbitrary colors** → ✅ Semantic color system
3. ❌ **Inconsistent navigation** → ✅ Predictable patterns
4. ❌ **No feedback** → ✅ Status messages and highlights
5. ❌ **Dense information** → ✅ Progressive disclosure
6. ❌ **No error recovery** → ✅ Graceful error handling
7. ❌ **Magic numbers** → ✅ Named constants
8. ❌ **Global state** → ✅ Context-based state management

### Manual Testing Scenarios

**Basic Navigation**

1. Start application
2. Navigate all screens using sidebar
3. Navigate all screens using number keys
4. Use ESC to go back
5. Use F1 for help
6. Quit with Q

**List Interaction**

1. Navigate list with arrows
2. Select item
3. Scroll through long list
4. Search for item
5. Add new item
6. Edit selected item
7. Delete selected item

**Form Interaction**

1. Fill form fields
2. Navigate between fields
3. Validate input
4. Submit form
5. Cancel form
6. Handle errors

**Edge Cases**

1. Very small terminal (80×24)
2. Very large terminal (200×60)
3. Rapid resize
4. Empty lists
5. Long text in fields
6. Special characters in input
7. Network/file errors

### Future Evolution Ideas

1. **Themes**: Multiple color themes (dark/light/high-contrast)
2. **Customization**: User-configurable shortcuts
3. **Plugins**: Extensible component system
4. **Multi-language**: i18n support
5. **Accessibility**: Screen reader support
6. **Performance**: Async operations with progress
7. **Undo/Redo**: Action history
8. **Search**: Global search across all data

---

**This specification serves as the foundation for implementing a reference-quality TUI.**
