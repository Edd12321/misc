#include <stdio.h>
#include <stdlib.h>
#include <string.h>
unsigned char mem[30000], *dp = mem;

struct instr {
	enum {
		INC_DP, INC_BY, DEC_DP, DEC_BY,
		OUT, IN, FORWARD, BACKWARD, HALT
	} cmd;
	struct instr *arg;
};

struct stack {
	struct stack *last, *next;
	struct instr *pos;
};

static inline void
eval(FILE *in)
{
	/* Phase 1: parse file */
	fseek(in, 0L, SEEK_END);
	size_t len = ftell(in);
	rewind(in);
	struct stack *stk = malloc(sizeof(struct stack));
	struct instr *buf = malloc(sizeof(struct instr) * len), *pc = buf;
	int ch;
	while ((ch = fgetc(in)) != EOF) {
		switch (ch) {
		case '<': pc++->cmd = DEC_DP;  break;
		case '>': pc++->cmd = INC_DP;  break;
		case '-': pc++->cmd = DEC_BY;  break;
		case '+': pc++->cmd = INC_BY;  break;
		case ',': pc++->cmd = IN;      break;
		case '.': pc++->cmd = OUT;     break;
		case '[':
			pc->cmd = FORWARD;
			stk->next = malloc(sizeof(stk));
			stk->next->pos = pc++;
			stk->next->next = NULL;
			stk->next->last = stk;
			stk = stk->next;
			break;
		case ']':
			pc->cmd = BACKWARD;
			pc->arg = stk->pos+1;
			stk->pos->arg = ++pc;
			stk = stk->last;
			free(stk->next);
			stk->next = NULL;
			break;
		}
	}
	/* Phase 2: execute instructions */
	for (pc->cmd = HALT, pc = buf;;) {
		switch (pc->cmd) {
		case BACKWARD: pc = *dp ? pc->arg : pc+1; break;
		case FORWARD:  pc = *dp ? pc+1 : pc->arg; break;
		case IN:           *dp = getchar(); ++pc; break;
		case OUT:             putchar(*dp); ++pc; break;
		case DEC_BY:                 --*dp; ++pc; break;
		case INC_BY:                 ++*dp; ++pc; break;
		case DEC_DP:                  --dp; ++pc; break;
		case INC_DP:                  ++dp; ++pc; break;
			break;
		default:
			goto _stop;
		}
	}
_stop:
	free(buf);
}

int
main(int argc, char *argv[])
{
	FILE *fp;
	if (argc != 2 || !(fp = fopen(argv[1], "r"))) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	eval(fp);
	exit(EXIT_SUCCESS);
}
