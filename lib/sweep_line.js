// S. Tokumine 18/04/2011
//
// Javascript port of http://softsurfer.com/Archive/algorithm_0108/algorithm_0108.htm
//
// The Intersections for a Set of 2D Segments, and Testing Simple Polygons
//
// Shamos-Hoey Algorithm implementation in Javascript
//

var RedBlackTree = require('./red_black_tree');
var Point = require('./point');

// A container class for segments (or edges) of the polygon to test
// Allows storage and retrieval from the Balanced Binary Tree
function SweepLineSeg(ev) {
	this.edge = ev.edge;            // polygon edge i is V[i] to V[i+1]
	this.leftPoint = null;         // leftmost vertex point
	this.rightPoint = null;        // rightmost vertex point
	this.above = null;              // segment above this one
	this.below = null;              // segment below this one
}

// required comparator for binary tree storage. Sort by y axis of the
// points where the segment crosses L (eg, the left point)
SweepLineSeg.prototype.compare = function (seg) {
	return this.equal(seg) ? 0 : (seg.lessThan(this) ? -1 : 1);
};

// return true if 'this' is below 'seg'
SweepLineSeg.prototype.lessThan = function (seg) {
	// First check if these two segments share a left vertex
	if (this.leftPoint == seg.leftPoint) {
		// Same point - the two segments share a vertex.
		// use y coord of right end points
		return this.rightPoint.y < seg.rightPoint.y;
	}
	return Point.prototype.isLeft(this.leftPoint, this.rightPoint, seg.leftPoint) > 0
};

SweepLineSeg.prototype.equal = function (seg) {
	return this.edge == seg.edge
};

SweepLineSeg.prototype.toString = function () {
	return "edge:" + this.edge;
};


// Main SweepLine class.
// For full details on the algorithm used, consult the C code here:
// http://softsurfer.com/Archive/algorithm_0108/algorithm_0108.htm
//
// This is a direct port of the above C to Javascript
function SweepLine(polygon) {
	this.tree = new RedBlackTree();
	this.numVx = polygon.vertices.length;
	this.polygon = polygon;
}

// Add Algorithm 'event' (more like unit of analysis) to queue
// Units are segments or distinct edges of the polygon.
SweepLine.prototype.add = function (ev) {

	// build up segment data
	var seg = new SweepLineSeg(ev);
	ev.seg = seg;
	var p1 = this.polygon.vertices[seg.edge];
	var p2 = seg.edge + 1 < this.polygon.vertices.length ? this.polygon.vertices[seg.edge + 1] : this.polygon.vertices[0];

	// if it is being added, then it must be a LEFT edge event
	// but need to determine which endpoint is the left one first
	if (p1.compare(p2) < 0) {
		seg.leftPoint = p1;
		seg.rightPoint = p2;
	} else {
		seg.leftPoint = p2;
		seg.rightPoint = p1;
	}

	// Add node to tree and setup linkages to "above" and "below"
	// edges as per algorithm
	var nd = this.tree.add(seg);
	var nx = this.tree.findNext(nd._value);
	var np = this.tree.findPrevious(nd._value);

	if (nx) {
		seg.above = nx;
		seg.above.below = seg;
	}
	if (np) {
		seg.below = np;
		seg.below.above = seg;
	}

	return seg;
};


SweepLine.prototype.find = function (ev) {

	// need a segment to find it in the tree
	var seg = new SweepLineSeg(ev);
	var p1 = this.polygon.vertices[seg.edge];
	var p2 = this.polygon.vertices[seg.edge + 1];
	// if it is being added, then it must be a LEFT edge event
	// but need to determine which endpoint is the left one first
	if (p1.compare(p2) < 0) {
		seg.leftPoint = p1;
		seg.rightPoint = p2;
	} else {
		seg.leftPoint = p2;
		seg.rightPoint = p1;
	}
	return this.tree.find(seg)
};

// When removing a node from the tree, ensure the above and below links are
// passed on to adjacent nodes before node is deleted
SweepLine.prototype.remove = function (seg) {

	// Pretty sure there is a bug here as the tree isn't getting pruned correctly.
	// In fact, I thin the remove method is removing the wrong elements from the list.
	//

	var nd = this.tree.find(seg);
	if (!nd) return;

	// get the above and below segments pointing to each other
	var nextNode = this.tree.findNext(nd);
	if (nextNode) {
		nextNode.below = seg.below;
	}

	var prevNode = this.tree.findPrevious(nd);
	if (prevNode) {
		prevNode.above = seg.above;
	}

	// now  can safely remove it
	this.tree.remove(nd);
};

// test intersect of 2 segments and return: false=none, true=intersect
SweepLine.prototype.intersect = function (s1, s2) {
	if (!s1 || !s2) return false; // no intersect if either segment doesn't exist

	// check for consecutive edges in polygon
	var e1 = s1.edge;
	var e2 = s2.edge;

	if (((e1 + 1) % this.numVx === e2) || (e1 === (e2 + 1) % this.numVx))
		return false; // no non-simple intersect since consecutive

	// test for existence of an intersect point
	var lsign = Point.prototype.isLeft(s1.leftPoint, s1.rightPoint, s2.leftPoint); // s2 left point sign
	var rsign = Point.prototype.isLeft(s1.leftPoint, s1.rightPoint, s2.rightPoint); // s2 right point sign
	if (lsign * rsign > 0) // s2 endpoints have same sign relative to s1
		return false; // => on same side => no intersect is possible

	lsign = Point.prototype.isLeft(s2.leftPoint, s2.rightPoint, s1.leftPoint); // s1 left point sign
	rsign = Point.prototype.isLeft(s2.leftPoint, s2.rightPoint, s1.rightPoint); // s1 right point sign
	if (lsign * rsign > 0) // s1 endpoints have same sign relative to s2
		return false; // => on same side => no intersect is possible

	return true; // segments s1 and s2 straddle. Intersect exists.
};

module.exports = SweepLine;
