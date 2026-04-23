# Debugging Reference

## Workflow

Start QEMU (freezes CPU, waits for GDB) in one terminal:
```
make startqemu
```

Connect GDB in a second terminal:
```
make connectgdb
```

QEMU serial output appears in the first terminal. GDB runs in the second.

---

## GDB TUI

### Layouts

| Command | Effect |
|---|---|
| `layout asm` | Disassembly pane |
| `layout src` | Source pane |
| `layout regs` | Add register pane above current layout |
| `layout split` | Source + disassembly together |

### Register pane views

| Command | Effect |
|---|---|
| `tui reg general` | General-purpose registers (x0–x31 / ABI names) |
| `tui reg all` | All registers |
| `tui reg float` | Floating-point registers |

### Switching views mid-session

Stock GDB TUI supports two panes at most. The default layout (registers +
assembly) and source view are mutually exclusive — use these to switch between
them during a session:

| Command | Effect |
|---|---|
| `layout src` | Switch to source view (replaces register pane) |
| `layout regs` | Switch back to registers + assembly (default) |

`layout src` only shows content when the PC is inside a function compiled from
a `.c` file. It will be blank when stopped in assembly-only code (e.g. `boot.S`).

### Navigation

| Command | Effect |
|---|---|
| `focus cmd` | Keyboard focus to command prompt |
| `focus asm` | Keyboard focus to disassembly pane (arrow keys scroll) |
| `focus regs` | Keyboard focus to register pane |
| `Ctrl-X A` | Toggle TUI on/off |
| `Ctrl-L` | Refresh display (use when layout gets garbled) |

---

## .gdbinit

GDB executes `.gdbinit` on startup. A project-level `.gdbinit` requires GDB to
trust the directory, configured via `set auto-load safe-path`. This is passed
via `-iex` in the `connectgdb` Makefile target so it works on any machine
without touching `~/.gdbinit`.

Current `.gdbinit`:
```
layout asm
layout regs
tui reg general
focus cmd
```

---

## Useful GDB commands for bare-metal

| Command | Effect |
|---|---|
| `stepi` / `si` | Step one instruction |
| `nexti` / `ni` | Step one instruction, skip over calls |
| `continue` / `c` | Run until next breakpoint |
| `break _start` | Set breakpoint at label |
| `info registers` | Print all registers (non-TUI fallback) |
| `x/10i $pc` | Disassemble 10 instructions from current PC |
| `x/4wx 0x80000000` | Examine memory as 4 hex words at address |
| `set $pc = 0x80000000` | Manually set PC |
| `monitor system_reset` | Reset the QEMU VM from GDB |
