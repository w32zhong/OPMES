#include "common.h"
#include "enum-symbol.h"
#include "enum-token.h"
#include "tex-tr.h"

int main()
{
	uint32_t n;
	struct list_it li_subpath;
	int subpath_err;

	trace_init("test-tex-tr.log"); 
	struct tex_tr *tr = tex_tr_alloc(100, 0x2);
	struct tex_tr *a = tex_tr_alloc(1, 0x2);
	struct tex_tr *b = tex_tr_alloc(2, 0x1);
	struct tex_tr *c = tex_tr_alloc(11, T_FRAC);
	struct tex_tr *d = tex_tr_alloc(3, 0x3);
	struct tex_tr *e = tex_tr_alloc(31, 0x1);
	struct tex_tr *f = tex_tr_alloc(32, 0x1);
	struct tex_tr *g = tex_tr_alloc(32, 0x1);
	struct tex_tr *h = tex_tr_alloc(34, 0x2);
	struct tex_tr *i = tex_tr_alloc(32, 0x1);
	struct tex_tr *j = tex_tr_alloc(32, 0x1);
	struct tex_tr *nil0 = tex_tr_alloc(S_NIL, T_NIL);
	struct tex_tr *nil1 = tex_tr_alloc(S_NIL, T_NIL);
	struct tex_tr *nil2 = tex_tr_alloc(S_NIL, T_NIL);

	tex_tr_attatch(d, e);
	tex_tr_attatch(d, f);
	tex_tr_attatch(d, g);
	tex_tr_attatch(d, nil0);
	tex_tr_attatch(d, nil1);
	tex_tr_attatch(tr, d);

	tex_tr_attatch(c, h);
	tex_tr_attatch(c, j);

	tex_tr_attatch(a, c);
	tex_tr_attatch(tr, a);
	tex_tr_attatch(tr, b);
	tex_tr_attatch(tr, i);
	tex_tr_attatch(tr, nil2);

	printf("raw tree:\n");
	tex_tr_print(tr, stdout);

	tex_tr_group(tr);
	printf("after group:\n");
	tex_tr_print(tr, stdout);
	
	n = tex_tr_prune(tr);
	printf("after pruning: (%d pruned)\n", n);
	tex_tr_print(tr, stdout);
	
	n = tex_tr_assign(tr);
	printf("after assign: (%d branch words in total)\n", n);
	tex_tr_print(tr, stdout);

	li_subpath = tex_tr_subpaths(tr, &subpath_err);

	if (subpath_err) {
		printf("subpath generation error.\n");
	} else {
		printf("subpaths:\n");
		subpaths_print(&li_subpath, stdout);
	}

	subpaths_free(&li_subpath);

	tex_tr_release(tr);

	trace_unfree();
	trace_uninit();
	return 0;
}
