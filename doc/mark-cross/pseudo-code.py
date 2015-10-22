# parser:
# input: url, tex list
# output: url, (tex, tree, brw[])[]

# index:
# input: parser.output
# output: 
#	./col/bdb {#id}->{#brw, url, tex, tree}
#	./col/path/to/brw/posting {max, min, records[]}

# search:
def score_tr_mark_cross(li_query_brw, score_tr):
	i = 0
	for q_brw in li_query_brw:
		for pointer in q_brw.gutter:
			if pointer->state == unmark:
				score = score_cache(q_brw.id, pointer->brw_id)
				if score > pointer->father.max[i]:
					pointer->father.max[i] = score
					pointer->father.who[i] = pointer
					pointer->state = mark
			i += 1

		stage_flag = 0
		if last_iteration():
			stage_flag = 1
		else if next_vname() != brw.vname:
			stage_flag = 1

		if stage_flag:
			max_var_score = 0
			max_var = None
			for var in score_tr.root:
				var_score = sum(var.max)
				if var_score > max_var_score:
					max_var_score = var_score
					max_var = var
			score_tr.root.score += max_var_score
			for var in score_tr.root:
				test = (var == max_var)
				next_state = cross if test else unmark
				for who in var.who:
					who.state = next_state
				clear(var)

# score tree: 
#	root: { score, n_var}
#	var[]: { n_leaf, vname, {max, who}[n_query_brw] }
#	leaf: { father, state }

def merge_search(li_search_path, li_query_brw, score_tr, cnt):
	given matched doc_brw in li_search_path[i]:
		if doc_brw.id in ranking_set:
			continue
		score_tr_clean(score_tr)
		for query_brw in li_search_path[i].src:
			n_query_brw = len(li_query_brw)
			score_leaf = score_tr_insert(score_tr, doc_brw, 
			                             n_query_brw)
			query_brw.gutter.add_pointer(score_leaf) 
			score_cache(doc_brw, query_brw)
	given doc at the end of matching doc:
		score_tr_mark_cross(li_query_brw, score_tr)
		add_to_ranking(score_tr.root.score, doc.id)
		cnt += 1
		if cnt >= threshold:
			return false	

# brw: { nu, vname, weight[] }
# query_brw: { brw, dir, gutter }
# search_path: { dir, src[] }

def main():
	li_merge_point = merge_points(tr_query) # in depth order
	cnt = 0
	score_tr = new_score_tr() 
	for mp in li_merge_point:
		li_query_brw = query_brw(mp)
		li_search_path = search_path(li_query_brw) # unique path
		# each search path may contain multiple source query brw(s)
		li_query_brw = sort_by_vname(li_query_brw)
		if not merge_search(li_search_path, li_query_brw, 
		                    score_tr, &cnt)
			break
	score_tr_destroy(score_tr)
