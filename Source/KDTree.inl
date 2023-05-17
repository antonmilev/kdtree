// --------------------------------------------
template <typename T>
KDTree<T>::KDNode::KDNode(const T* x, int n, int axis)
{
    this->x.resize(n);
	this->axis = axis;
	for(int k=0; k<n; k++)
		this->x[k] = x[k];

	left = right = parent = nullptr ;
	checked = false ;
	id = 0;
}

// --------------------------------------------
template <typename T>
typename KDTree<T>::KDNode* KDTree<T>::find_parent(const T* x0)
{
	KDNode* parent ;
	int split ;
    KDNode* node = root;
	while(node)
	{
		split = node->axis  ;
		parent = node ;
		if(x0[split] > node->x[split])
			node = node->right ;
		else
			node = node->left ;
	}
	return parent ;
}

template <typename T>
typename KDTree<T>::KDNode*	KDTree<T>::insert(const T* p)
{
	KDNode* parent = find_parent(p);
	if(equal(p, parent->x.data(), n_dim))
		return nullptr ;

	KDNode* newNode = new KDNode(p, n_dim, parent->axis +1 < n_dim? parent->axis+1:0);
	newNode->parent = parent ;

	if(p[parent->axis] > parent->x[parent->axis])
	{
		parent->right = newNode ;
	}
	else
	{
		parent->left = newNode ;
	}

	return newNode ;
}

template <typename T>
KDTree<T>::KDTree(unsigned int K, unsigned int D) : root(nullptr), n_dim(D)
{
	KD_id = 1;
    NodesList.reserve(K);
    NodesChecked.reserve(K);

    x_min.resize(D);
    x_max.resize(D);
    max_boundary.resize(D); 
    min_boundary.resize(D);
}

template <typename T>
KDTree<T>::~KDTree()
{
    clear();
}

template <typename T>
void KDTree<T>::clear()
{
    for (auto p : NodesList)
        delete(p);

    NodesList.clear();
}

template <typename T>
bool KDTree<T>::add(const T* x)
{
	if(!root)
	{
		root =  new KDNode(x,n_dim,0);
		root->id = KD_id++ ;
		NodesList.push_back(root) ;
	}
	else
	{
		KDNode* node ;
		if((node=insert(x)))
		{
			node->id = KD_id++ ;
			NodesList.push_back(node);
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
	d_min = distance2(root->x.data(), x, n_dim);
	for(int n=1; n<NodesList.size(); n++)
	{
		d =  distance2(NodesList[n]->x.data(), x, n_dim);
		if(d < d_min)
		{
			nearest = NodesList[n] ;
			d_min = d;
		}
	}

	return nearest ;
}

//sequential exact match search
template <typename T>
typename KDTree<T>::KDNode* KDTree<T>::find_exact_brute(const T* x) 
{
	KDNode* exact = nullptr ;
	for(int n=1; n<NodesList.size(); n++)
	{
        if (equal(NodesList[n]->x.data(), x, n_dim))
            return NodesList[n];
	}

	return exact ;
}

template <typename T>
typename KDTree<T>::KDNode* KDTree<T>::find_nearest(const T* x)
{
	if(!root)
		return nullptr ;

	KDNode* parent = find_parent(x);
	nearest_neighbour = parent ;
	d_min = distance2(x, parent->x.data(), n_dim); 

	if(equal(x, parent->x.data(), n_dim))
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

    KDNode* parent = find_parent(x);
    while (parent)
    {
        if (equal(x, parent->x.data(), n_dim))
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

	//NodesChecked[checked_nodes++] = node ;

    NodesChecked.push_back(node);
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
	for(int k=0; k<n_dim; k++)
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
	for(int k=0; k<n_dim; k++)
	{
		x_min[k] = x_max[k] = 0;
		max_boundary[k] = min_boundary[k] = 0;
	}
	n_boundary = 0;

	KDNode* search_root = parent ;
    // here we go up in the tree until a bounding box around the test point x is checked 
	while(parent && n_boundary != 2*n_dim)
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
    for(auto& n : NodesChecked)
		n->checked = false;
    NodesChecked.clear();
}
