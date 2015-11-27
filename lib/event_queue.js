var iterIndex;
function EventQueue(polygon) {
	var pn = polygon.vertices.length; // point numbers
	iterIndex = 0;
	this.numberOfEvents = 2 * pn;  // 2 vertex events for each edge
	this.events = [];

	// Initialize event queue with edge segment endpoints
	for (var i = 0; i < pn; i++) {// init data for edge i
		var a = 2 * i;
		var b = 2 * i + 1;
		this.events[a] = {edge: i};
		this.events[b] = {edge: i};
		this.events[a].vertex = polygon.vertices[i];
		this.events[b].vertex = i + 1 < pn ? polygon.vertices[i + 1] : polygon.vertices[0];

		this.events[a].otherEnd = this.events[b];
		this.events[b].otherEnd = this.events[a];

		this.events[a].seg = this.events[b].seg = 0;

		if (this.events[a].vertex.compare(this.events[b].vertex) < 0) { // determine type
			this.events[a].type = 'left';
			this.events[b].type = 'right';
		} else {
			this.events[a].type = 'right';
			this.events[b].type = 'left';
		}
	}

	// sort events lexicographically
	this.events.sort(compare);
}

EventQueue.prototype.next = function () {
	var result = this.events[iterIndex];
	if (result) {
		iterIndex++;
	}
	return result
};

EventQueue.prototype.resetIterator = function () {
	iterIndex = 0;
};

function compare(e1, e2) {
	var v1 = e1.vertex,
		v2 = e2.vertex;

	var r = v1.compare(v2);

	if (r == 0) {
		if (e1.type == e2.type) return 0;
		if (e1.type == 'left') return -1;
		else return 1;
	} else {
		return r;
	}
}

module.exports = EventQueue;
