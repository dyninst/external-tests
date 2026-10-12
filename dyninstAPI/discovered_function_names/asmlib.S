// A library whose functions the parser can only reach by following control
// flow, so that it exercises how a discovered function gets its name.
//
// Hand-written assembly routinely labels an entry point without typing it.
// The assembler emits a plain label as an STT_NOTYPE symbol, which Symtab does
// not aggregate into a Function, so no parse ever gets a hint for it.

	.text

	.globl  asm_entry
	.type   asm_entry, @function
asm_entry:
	pushq   %rbp
	call    labelled_helper
	movl    %eax, %ebp
	call    1f
	addl    %ebp, %eax
	popq    %rbp
	ret
	.size   asm_entry, .-asm_entry

// Named, but untyped: a symbol is in the table and it is the name the author
// gave this code.
labelled_helper:
	movl    $7, %eax
	ret

// Not named at all: nothing in the symbol table sits at this address.
1:
	movl    $9, %eax
	ret

	.section .note.GNU-stack,"",@progbits
