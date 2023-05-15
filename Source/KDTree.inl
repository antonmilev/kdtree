// --------------------------------------------
template <typename T>
KDTree<T>::KDNode::KDNode(const T* x, int axis0)
{
	axis = axis0;
	for(int k=0; k<SD; k++)
		this->x[k] = x[k];

	left = right = parent = nullptr ;
	checked = false ;
	id = 0;
}

template <typename T>
typename KDTree<T>::KDNode* KDTree<T>::KDNode::find_parent(const T* x0)
{
	KDNode* parent ;
	KDNode* next = this ;
	int split ;
	while(next)
	{
		split = next->axis  ;
		parent = next ;
		if(x0[split] > next->x[split])
			next = next->right ;
		else
			next = next->left ;
	}
	return parent ;
}

template <typename T>
typename KDTree<T>::KDNode*	KDTree<T>::KDNode::insert(const T* p)
{
	KDNode* parent = find_parent(p);
	if(equal(p, parent->x, SD))
		return nullptr ;

	KDNode* newNode = new KDNode(p, parent->axis +1 < SD? parent->axis+1:0);
	newNode->parent = parent ;

	if(p[parent->axis] > parent->x[parent->axis])
	{
		parent->right = newNode ;
		newNode->orientation = 1 ;
	}
	else
	{
		parent->left = newNode ;
		newNode->orientation = 0 ;
	}

	return newNode ;
}

// --------------------------------------------
template <typename T>
KDTree<T>::KDTree()
{
	root = nullptr ;
	KD_id = 1;
	nList = 0;
}

template <typename T>
KDTree<T>::~KDTree()
{
    clear();
}

template <typename T>
void KDTree<T>::clear()
{
    while (nList--)
        delete List[nList];
    nList = 0;
}

template <typename T>
bool KDTree<T>::add(const T* x)
{
	if(nList >= KD_MAX_POINTS)
		return false; //can't add more points
	
	if(!root)
	{
		root =  new KDNode(x,0);
		root->id = KD_id++ ;
		List[nList++] = root ;
	}
	else
	{
		KDNode* pNode ;
		if((pNode=root->insert(x)))
		{
			pNode->id = KD_id++ ;
			List[nList++] = pNode;
		}
	}

	return true ;
}

//sequential nearest neighbour search
template <typename T>
typename KDTree<T>::KDNode* KDTree<T>::find_nearest_brute(const T* x) 
{
	if(!root)
		return nullptr;

	KDNode* nearest = root ;
	T d ;
	d_min = distance2(root->x, x, SD);
	for(int n=1; n<nList; n++)
	{
		d =  distance2(List[n]->x, x, SD);
		if(d < d_min)
		{
			nearest = List[n] ;
			d_min = d;
		}
	}

	return nearest ;
}

template <typename T>
typename KDTree<T>::KDNode* KDTree<T>::find_nearest(const T* x)
{
	if(!root)
		return nullptr ;

	checked_nodes = 0;

	KDNode* parent = root->find_parent(x);
	nearest_neighbour = parent ;
	d_min = distance2(x, parent->x, SD); ;

	if(equal(x, parent->x, SD))
		return nearest_neighbour ;

	search_parent(parent, x);
	uncheck();

	return nearest_neighbour ;
}

//------------------------------------------------------------------------------------------------------------------
template <typename T>
typename KDTree<T>::KDNode* KDTree<T>::find_exact(const T* x) 
{
    if(!root)
        return nullptr ;

    KDNode* parent = root->find_parent(x);

    while (parent)
    {
        if (equal(x, parent->x, SD))
            return parent;

        parent = parent->parent;
    }
  
	return nullptr ;
}

template <typename T>
void KDTree<T>::check_subtree(KDNode* node, const T* x)
{
	if(!node || node->checked)
		return ;

	CheckedNodes[checked_nodes++] = node ;
	node->checked = true ;
	set_bounding_cube(node, x);
	
	int dim = node->axis ;
	T d= node->x[dim] - x[dim];

	if (d*d > d_min) {
		if (node->x[dim] > x[dim])
		  check_subtree(node->left, x);
		else 
		  check_subtree(node->right, x);
	}
	// If the distance from the key to the current value is 
	// less than the nearest distance, we still need to look
	// in both directions.
	else {
		check_subtree(node->left, x);
		check_subtree(node->right, x);
	}
}

template <typename T>
void KDTree<T>::set_bounding_cube(KDNode* node, const T* x)
{
	if(!node)
		return ;
	int d=0;
	T dx;
	for(int k=0; k<SD; k++)
	{
		dx = node->x[k]-x[k];
		if(dx > 0)
		{
			dx *= dx ;
			if(!max_boundary[k])
			{
				if(dx > x_max[k])
					x_max[k] = dx;
				if(x_max[k]>d_min)
				{
					max_boundary[k] =true;
					n_boundary++ ;
				}
			}
		}
		else 
		{
			dx *= dx ;
			if(!min_boundary[k])
			{
				if(dx > x_min[k])
					x_min[k] = dx;
				if(x_min[k]>d_min)
				{
					min_boundary[k] =true;
					n_boundary++ ;
				}
			}
		}
		d+=dx;
		if(d>d_min)
			return ;

	}
	
	if(d<d_min)
	{
		d_min = d;
		nearest_neighbour = node ;
	}
}

template <typename T>
typename KDTree<T>::KDNode* KDTree<T>::search_parent(KDNode* parent, const T* x)
{
	for(int k=0; k<SD; k++)
	{
		x_min[k] = x_max[k] = 0;
		max_boundary[k] = min_boundary[k] = 0;
	}
	n_boundary = 0;

	T dx;
	KDNode* search_root = parent ;
    // here we go up in the tree until a bounding box around the test point x is checked 
	while(parent && n_boundary != 2*SD)
	{	
		check_subtree(parent, x);
		search_root = parent ;
		parent = parent->parent ;
	}

	return search_root ;
}

template <typename T>
void KDTree<T>::uncheck()
{
	for(int n=0; n<checked_nodes; n++)
		CheckedNodes[n]->checked = false;
}
