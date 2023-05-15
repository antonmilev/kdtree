# kdtree
<br>Demonstrates how to implement and use a C++ template of a k-d tree.

**Run the demo**
<p>This demo can be build for Windows and Linus.

**Windows**
<p>Just open and build the solution KD.sln with Visual Studio.

**Linux**
<p> g++ main.cpp -o kdtree


**Introduction**
<br><br>The <b>k-d tree</b> is a variant of <b>binary tree</b> in which every node contains a k-dimensional point and has a splitting hyperplane that divides the space into two parts. Hypeplane direction is selected to rotate along the d-dimensions. For full explanation [see](https://en.wikipedia.org/wiki/K-d_tree) . 

The <b>k-d tree</b> can be used for many problems with low dimensional data (d < 10), it allows some balancing techniques and effectibe Nearest-Neighbours search, which makes it useful even for 3D and 2D (for other spatial trees like OcTree balancing and nearest-neighbours search is even more challenging). Here I have demonstrated how  Nearest-Neighbours search can be efficiently organized. 

<pre><code>However, I have another option.
I can use a code block that works very nicely 
but I can not <b>bold text</b> a particular part of the code
<a href="https://meta.stackexchange.com/q/243709">I want to stand out.</a>
Just to make it easier to locate.
Nor can I make the font smaller and get rid of that ugly scroll bar. It's just slightly too wide.</code></pre>

**Implementation of the KdTree template in C/C++**


```python
import numpy as np
import datetime
from tensorflow.keras.models import Sequential 
from tensorflow.keras.layers import Dense   
from tensorflow.keras import optimizers
import tensorflow as tf
from sklearn.metrics import accuracy_score
import matplotlib.pyplot as plt

```


```cpp
int main() {
  int y = SOME_MACRO_REFERENCE;
  int x = 5 + 6;
  cout << "Hello World! " << x << std::endl();
}
```


**Nearest Neighbours Search**
**Benchmark comparison**