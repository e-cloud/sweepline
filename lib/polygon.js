var EventQueue = require('./event_queue'),
	SweepLine = require('./sweep_line');


function Polygon(point_array) {
	this.vertices = point_array
}

// Tests polygon simplicity.
// returns true if simple, false if not.
Polygon.prototype.isSimplePolygon = function () {

	var event_queue = new EventQueue(this);
	var sweep_line = new SweepLine(this);
	var seg, event;

	// This loop processes all events in the sorted queue
	// Events are only left or right vertices
	while (event = event_queue.next()) {
		if (event.type == 'left') {
			seg = sweep_line.add(event);
			if (sweep_line.intersect(seg, seg.above)) return false;
			if (sweep_line.intersect(seg, seg.below)) return false;

		} else {
			seg = event.otherEnd.seg;
			if (sweep_line.intersect(seg.above, seg.below)) return false;
			sweep_line.remove(seg);
		}
	}
	return true;
};

module.exports = Polygon;
