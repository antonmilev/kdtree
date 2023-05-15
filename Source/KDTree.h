#ifndef __KDTREE_H__
#define __KDTREE_H__

#define SD 7					 //current space dimension
#define KD_MAX_POINTS 2000000    //max KD tree points number  

template <typename T> 
T distance2(const T* x1, const T* x2, int dim)
{
	T S = 0;
	for(int k=0; k < dim; k++)
		S+= (x1[k]-x2[k])*(x1[k]-x2[k]);
	return S;
}

template <typename T>
bool equal(const T* x1, const T* x2, int dim)
{
	for(int k=0; k < dim; k++)
	{
		if(x1[k] != x2[k])
			return false;
	}

	return true ;
}

template <typename T>
class KDTree
{
public:
    class KDNode
    {
    //member functions
    public:
	    int axis ;
	    T x[SD];
	    unsigned int id ;
	    bool checked ;
	    bool orientation ;

        KDNode(const T* x, int axis0);

        KDNode*	insert(const T* x) ;
        KDNode*	find_parent(const T* x) ;

	    KDNode* parent ;
	    KDNode* left ;
	    KDNode* right ;
    };

	KDNode*  root ;
	KDTree();
    ~KDTree();
	bool			add(const T* x);
    KDNode*			find_exact(const T* x) ;
	KDNode*			find_nearest(const T* x);
	KDNode*			find_nearest_brute(const T* x) ;
    void            clear();

public:
	int				checked_nodes ;
	T				d_min ;    
	int				nList ;

protected:	
	void		    check_subtree(KDNode* node, const T* x);
	void		    set_bounding_cube(KDNode* node, const T* x);
	KDNode*	        search_parent(KDNode* parent, const T* x);
	void			uncheck();

	KDNode*		    nearest_neighbour ;
	int				KD_id  ;
	KDNode*		    List[KD_MAX_POINTS] ;

	KDNode*		    CheckedNodes[KD_MAX_POINTS] ;
	T				x_min[SD], x_max[SD]; 
	bool			max_boundary[SD], min_boundary[SD];
	int				n_boundary ;
};

#include "KDTree.inl"

#endif