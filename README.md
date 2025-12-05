# TISC: Ternary Symbol Instruction Set Computer and INS Language Compiler

**🙏TISC Report.pdf를 참고해주세요.**
**🙏TISC-INS Visualizer.html을 통해 프로그램 가동 구조를 살펴볼 수 있습니다.**

**Author:** Seung-U Son (Dept. of Computer Science and Engineering, Konkuk University)  
**Project Date:** 2025-12-05

---

## 1. Introduction

This project proposes **TISC (Ternary Symbol Instruction Set Computer)**, a novel minimalism computing architecture, and presents a compiler for its associated programming language, **INS**.

Inspired by the philosophy of **OISC (One Instruction Set Computer)** and the **Minsky Register Machine**, TISC operates using only three symbols: **I (Increment)**, **N (Move Next)**, and **S (Skip)**. Despite this extreme constraint, the system achieves **Turing Completeness** through a unique mechanism combining a **Circular Tape** and **Conditional Skip** logic.

This repository contains the formal specification of the TISC architecture, the mathematical proof of its computational power, and a fully functional compiler that translates INS source code into **x86-64 Assembly** (Windows/MSYS2 environment).

---

## 2. Architecture Definition

The TISC machine is formally defined as a 5-tuple mathematical model $M = <T_d, T_p, p, pc, \Sigma>$:

* **$T_d$ (Data Tape):** A circular tape storing integers ($\mathbb{Z}$). Each cell is defined in a finite integer space $\mathbb{Z}_m$ with a sufficiently large modulus $M$.
* **$T_p$ (Program Tape):** A circular tape of length $L$ storing instructions. The program counter ($pc$) wraps around to 0 upon reaching $L$.
* **$p$ (Data Pointer):** A pointer indicating the current cell on $T_d$.
* **$pc$ (Program Counter):** An index indicating the current instruction on $T_p$.
* **$\Sigma$ (Alphabet):** The instruction set $\{I, N, S\}$.

### 2.1. Instruction Set ($\Sigma$)

The TISC architecture utilizes a **Zero-Operand** format. Each symbol performs a single, atomic operation.

| Symbol | Action | Description |
| :--- | :--- | :--- |
| **I** | $Val \leftarrow Val + 1$ | Increments the value at the current data pointer. |
| **N** | $p \leftarrow p + 1$ | Moves the data pointer to the next cell (Right Shift). |
| **S** | **If** $Val == 0$, **Skip** | Conditional Skip. If the current value is 0, the machine skips the immediate next instruction ($pc \leftarrow pc + 2$). |

---

## 3. Theoretical Proof of Turing Completeness

This project mathematically proves that the TISC system is **Loose Turing Complete** via reduction to a **2-Counter Minsky Machine**, which is known to be Turing complete.

### 3.1. Lemma 2.2.1: Existence of Decrement via Overflow

Although $\Sigma$ only contains an increment instruction ($I$), a decrement operation ($DEC$) can be simulated utilizing the property of modular arithmetic on a finite tape. Subtracting 1 is mathematically equivalent to adding the additive inverse $M-1$:

$$
Val - 1 \equiv Val + M - 1 \pmod M
$$

Thus, executing $I$ exactly $M-1$ times results in a decrement.

### 3.2. Lemma 2.2.2: State-Guarded Execution

Using the conditional skip ($S$), a **Guard Mechanism** is implemented to execute specific code blocks only when the machine is in a specific state $k$. By calculating $\delta = vPC - k$ (where $vPC$ is the Virtual Program Counter stored at $T[0]$), the machine can conditionally invalidate or execute instructions based on the current state.

### 3.3. Theorem 2.2.3: Simulating Minsky Machine

The TISC machine $M$ can simulate any 2-Counter Minsky Machine $M_{Minsky}$ ($r1, r2, \{INC, DEC, JZ\}$) by mapping memory cells to registers and $vPC$. Since all essential components of $M_{Minsky}$ are implementable in $M$, it follows that $M \cong M_{Minsky} \cong \text{Turing Machine}$.

---

## 4. Compiler Implementation

The compiler translates INS source code into x86-64 assembly language, bridging the gap between the theoretical model and physical hardware.

### 4.1. Implementation Specifications

* **Pipeline:**
    * **Scanner (`scanner.l`):** Tokenizes the input stream into `TOKEN_I`, `TOKEN_N`, `TOKEN_S`.
    * **Parser (`parser.y`):** Validates syntax and constructs an Abstract Syntax Tree (AST).
    * **Code Generator (`codegen_x86.c`):** Translates the AST into x86 assembly.

* **Memory Model (8-bit Data Optimization):**
    To satisfy the **"Practicality of Decrement"**, the data tape is modeled as an 8-bit integer space ($\mathbb{Z}_{256}$). While a 64-bit model would require $2^{64}-1$ increments for a single decrement (an infeasible timeframe), the 8-bit model allows a decrement in just 255 increments, making the theoretical proof practically executable.

* **Halting Condition:**
    TISC is physically designed to execute infinitely (Circular Tape). However, for practical computation, the compiler implements a **Logical Halt**. It monitors the Virtual Program Counter at $T[0]$. If $T[0]$ reaches the terminal state $H = 255$ (`0xFF`), the emulator terminates execution and outputs the result.

* **System Compatibility:**
    Designed for **Windows x64 (MSYS2)**. It strictly adheres to the Microsoft x64 Calling Convention, including stack alignment and Shadow Space allocation (`subq $40, %rsp`) to ensure stability during I/O operations.

---

## 5. Usage and Execution

### Prerequisites
* OS: Windows 10/11 x64
* Environment: MSYS2 UCRT64
* Tools: `gcc`, `flex`, `bison`, `make`

### Build Instructions

```bash
# Build the compiler
$ make

# Clean build artifacts
$ make clean
```

### Compilation & Execution
1. **Compile INS code to Assembly:**
```bash
./INS_x86.exe source.ins
```
This generates an `out.s` file.

2. **Assemble and Link:**

```bash
make prog
```
This generates a `prog.exe` executable.

3. **Run:**
```bash
./prog.exe
```
---
## 6. Experimental Results
The compiler was verified using a `compliant_hello.ins` program. This program writes "Hello World!" to the tape and then sets the logic state $T[0]$ to 255 via pointer wrapping.

**Execution Output:**
```Plaintext
[TISC] System Started. Tape Length: 66876 (8-bit Mode)
[TISC] Logical Halt Detected (T[0] == 255). Result:
Hello World!
```
**(Note: $T[0] == 255$ indicates the value 255 interpreted as a signed 8-bit integer).**

---
## 7. Conclusion
This project successfully establishes the theoretical foundation of the minimalist TISC architecture and demonstrates its validity through a working compiler. By implementing mechanisms such as Decrement via Overflow and Logical Halting on Circular Tapes, it bridges abstract computation theory with practical system programming.

---
## 8. References
**Primary Source - Report**
- Son, S. (2025). Proposal of TISC (Ternary Symbol Instruction Set Computer) Based Language and Compiler. Dept. of Computer Science and Engineering, Konkuk University.

**Theoretical Foundations**
- Minsky, M. L. (1967). Computation: Finite and Infinite Machines. Prentice-Hall. (Regarding 2-Counter Minsky Machine)
- Turing, A. M. (1936). "On Computable Numbers, with an Application to the Entscheidungsproblem". Proceedings of the London Mathematical Society. (Regarding Turing Completeness and Halting Problem)

**Architecture & OISC**
- Mazonka, O., & Kolodin, A. (2011). "A Simple Multi-Processor Computer Based on Subleq". arXiv preprint arXiv:1106.2593. (Regarding Subleq OISC)
- Jones, F. (1988). "The Ultimate RISC". Computer Architecture News, 16(3), 48-55. (Regarding One Instruction Set Computer)

**Compiler Implementation**
- https://github.com/MinGiKYUNG/compiler_class.git
- Levine, J. (2009). flex & bison. O'Reilly Media. (Regarding Scanner/Parser Implementation)
