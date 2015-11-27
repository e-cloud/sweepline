Detecting polygon self-intersection in Javascript
============================================
An implementation of the O((n + k) log n) Bentley–Ottmann sweep-line algorithm for detecting crossings in a set of line segments. The aim was to make something to speedily detect self-intersecting polygons for client side validation before serialisation and storage, but there's not much to stop it being used server-side (except it being more a statement of intent than actual production ready code).  

Despite intending it for the browser, it probably still needs some fettling to make sure the CommonJS-style code I've written works in your target browser.

Reference Implementation
==============================================
http://geomalgorithms.com/a09-_avl_code.html#SweepLineClass 
the above is the implementation of Bentley–Ottmann sweep-line algorithm with the AVL tree.
There is a cpp file of that code in the `lib/`

But in this repo, we use the redblack tree to replace the AVL tree. It has some adjustment.
e.g. remove the `lpP` of Class `SweepLineSeg`.

Develop Environment
===========
* node.js 4.2 
* mocha 
* chai

Tests
======
To run the tests, ensure you have node.js and the expresso npm installed:

$ npm test

Caution
===========================================
Note that this implementation currently doesn't validate polygons that share the same start and end vertex. Look at the tests for workarounds to this issue.
If last vertex in geoJson is equal to first vertex, you need to remove the last one when you use the `isSimplePolygon`.

Licence
========
MIT
