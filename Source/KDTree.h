#ifndef __KDTREE_H__
#define __KDTREE_H__

#include <vector>

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
        friend class KDTree;
        public:
            KDNode(const T* x, int n, int axis);
            const T* data() const { return x.data(); }
            unsigned int getId() const { return id;  }

        protected:
	        KDNode* parent ;
	        KDNode* left ;
	        KDNode* right ;	       
            
            int axis ;
	        std::vector<T> x;
	        unsigned int id ;
	        bool checked ;
    };

	KDNode*  root ;
	KDTree(unsigned int K, unsigned int D);
    ~KDTree();
	bool			add(const T* x);
    KDNode*	        insert(const T* x) ;
    KDNode*			find_exact(const T* x) ;
	KDNode*			find_nearest(const T* x);
	KDNode*			find_nearest_brute(const T* x) ;
    KDNode*         find_exact_brute(const T* x);
    KDNode*	        find_parent(const T* x) ;
    void            clear();
    size_t          size() const { return NodesList.size();  };
    size_t          numChecked() const { return NodesChecked.size();  };
    
public:
	T				d_min ;   
    unsigned int    n_dim;

protected:	
	void		            check_subtree(KDNode* node, const T* x);
	void		            set_bounding_cube(KDNode* node, const T* x);
	KDNode*	                search_parent(KDNode* parent, const T* x);
	void			        uncheck();

	KDNode*		            nearest_neighbour ;
	int				        KD_id  ;
	std::vector<KDNode*>	NodesList ;
	std::vector<KDNode*>	NodesChecked ;
	std::vector<T>			x_min, x_max; 
	std::vector<bool>		max_boundary, min_boundary;
	int				        n_boundary ;
};

#include "KDTree.inl"

#endif