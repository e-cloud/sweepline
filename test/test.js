var sl = require('../lib'),
	EventQueue = sl.EventQueue,
	Polygon = sl.Polygon,
	Point = sl.Point,
	RedBlackTree = sl.RedBlackTree,
	EventQueue = sl.EventQueue,
	SweepLine = sl.SweepLine,
	assert = require('chai').assert;
var TestNumber = function (number) {
	this.value = number
};

TestNumber.prototype.compare = function (test_number) {
	if (this.value > test_number.value) return 1;
	if (this.value < test_number.value) return -1;
	return 0;
}
describe('sweepline', function () {
	describe('event queue', function () {
		it('test can create an EventQueue', function () {
			var geojson = [[100.0, 0.0], [101.0, 0.0], [101.0, 1.0], [100.0, 1.0], [100.0, 0.0]];
			var points = geojson.map(function (pnt) {
				return new Point(pnt[0], pnt[1]);
			});
			var polygon = new Polygon(points);
			var event_queue = new EventQueue(polygon)

			assert.equal(event_queue.events.length, 8);
			assert.equal(event_queue.events.length, event_queue.number_of_events);
		});
	});
	describe('point', function () {
		it('test less than comparator', function () {
			var p0 = new Point(1, 1);
			var p1 = new Point(3, 3);
			assert.equal(p0.compare(p1), -1);
		});

		it('test more than comparator', function () {
			var p0 = new Point(1, 1);
			var p1 = new Point(3, 3);
			assert.equal(p1.compare(p0), 1);
		});

		it('test equality', function () {
			var p0 = new Point(1, 1);
			assert.equal(p0.compare(p0), 0);
		});

		it('test left of line', function () {
			var p0 = new Point(1.0, 1.0);
			var p1 = new Point(3.0, 3.0);
			var p2 = new Point(1.0, 3.0);

			assert.ok(p2.is_left(p0, p1) > 0);
		});

		it('test right of line', function () {
			var p0 = new Point(1.0, 1.0);
			var p1 = new Point(3.0, 3.0);
			var p2 = new Point(3.0, 1.0);

			assert.ok(p2.is_left(p0, p1) < 0);
		});

		it('test on line', function () {
			var p0 = new Point(1.0, 1.0);
			var p1 = new Point(3.0, 3.0);
			var p2 = new Point(2.0, 2.0);

			assert.ok(p2.is_left(p0, p1) == 0);
		});
	});
	describe('polygon', function () {
		it('test can build a polygon from an array of points', function () {
			geom = [[100.0, 0.0], [101.0, 0.0], [101.0, 1.0], [100.0, 1.0], [100.0, 0.0]];
			points = geom.map(function (pnt) {
				return new Point(pnt[0], pnt[1]);
			});
			polygon = new Polygon(points);

			assert.equal(polygon.vertices.length, geom.length);
			assert.equal(polygon.vertices[0].x, geom[0][0]);
		});

		it('test is polygon simple 1', function () {

			// note hack on last co-ordinate.
			var geom = [[100.0, 0.0], [101.0, 0.0], [101.0, 1.0], [100.0, 1.0], [100.000001, 0.000001]];
			var points = geom.map(function (pnt) {
				return new Point(pnt[0], pnt[1]);
			});
			var polygon = new Polygon(points);

			assert.ok(polygon.simple_polygon(), "polygon is simple")
		});

		it('test is polygon simple 2', function () {
			var geom = [[2.0, 2.0], [1.0, 2.0], [1.0, 1.0], [2.0, 1.0], [3.0, 1.0], [3.0, 2.0], [2.000001, 2.000001]];
			var points = geom.map(function (pnt) {
				return new Point(pnt[0], pnt[1]);
			});
			var polygon = new Polygon(points);

			assert.ok(polygon.simple_polygon(), "polygon is simple")
		});

		it('test is polygon simple 3', function () {
			var geom = [[0, 0], [0, 1], [1, 1], [0, 1], [0.0001, 0.00001]];
			var points = geom.map(function (pnt) {
				return new Point(pnt[0], pnt[1]);
			});
			var polygon = new Polygon(points);

			assert.ok(polygon.simple_polygon(), "polygon is simple")
		});

		it('test is polygon simple 4', function () {
			var geom = [[2.0, 2.0], [2.0, 3.0], [3.0, 3.0], [4.0, 3.0], [4.0, 2.0], [2.000001, 2.00001]];
			var points = geom.map(function (pnt) {
				return new Point(pnt[0], pnt[1]);
			});
			var polygon = new Polygon(points);

			assert.ok(polygon.simple_polygon(), "polygon is complex")
		});


		it('test is polygon complex 1', function () {
			var geom = [[2.0, 2.0], [2.0, 3.0], [3.0, 1.0], [4.0, 3.0], [4.0, 2.0], [2.00001, 2.00001]];
			var points = geom.map(function (pnt) {
				return new Point(pnt[0], pnt[1]);
			});
			var polygon = new Polygon(points);

			assert.ok(!polygon.simple_polygon(), "polygon is complex")
		});


		it('test is polygon complex 2', function () {
			var geom = [[2.0, 2.0], [3.0, 2.0], [3.0, 3.0], [2.0, 3.0], [4.0, 2.0], [2.0000001, 2.000001]];
			var points = geom.map(function (pnt) {
				return new Point(pnt[0], pnt[1]);
			});
			var polygon = new Polygon(points);

			assert.ok(!polygon.simple_polygon(), "polygon is complex")
		});
	});
	describe('red black tree', function () {
		it('test add single', function () {
			var rbt = new RedBlackTree();
			rbt.add(new TestNumber(5));

			assert.equal(5, rbt.min().value, "First item should have value of 5.");
		});

		it('test TestNumber', function () {
			var one = new TestNumber(1);
			var two = new TestNumber(2);

			assert.equal(one.compare(two), -1);
			assert.equal(two.compare(one), 1);
			assert.equal(one.compare(one), 0);
		})

		it('test add multiple', function () {
			var rbt = new RedBlackTree();
			rbt.add(new TestNumber(5));
			rbt.add(new TestNumber(10));

			assert.equal(5, rbt.min().value, "First item should have value of 5.");
			assert.equal(10, rbt.max().value, "First item should have value of 5.");
		});


		it('test find test', function () {
			var rbt = new RedBlackTree();
			rbt.add(new TestNumber(5));
			assert.ok(rbt.find(new TestNumber(5)), "Tree should have item 5.");
			assert.ok(!rbt.find(new TestNumber(10)), "Tree should not have item 10.");
		});


		it('test should be able to remove first node', function () {
			var rbt = new RedBlackTree();
			rbt.add(new TestNumber(5));
			rbt.add(new TestNumber(10));
			rbt.add(new TestNumber(6));
			rbt.remove(new TestNumber(5))

			assert.equal(6, rbt.min().value, "root should now be 6");
			assert.ok(!rbt.find(new TestNumber(5)));
		});

		it('test should be able to remove two nodes', function () {
			var rbt = new RedBlackTree();
			rbt.add(new TestNumber(5));
			rbt.add(new TestNumber(10));
			rbt.add(new TestNumber(6));
			rbt.remove(new TestNumber(10));
			rbt.remove(new TestNumber(5));

			assert.equal(6, rbt.min().value, "root should now be 6");
			assert.ok(!rbt.find(new TestNumber(5)));
			assert.ok(!rbt.find(new TestNumber(10)));
		});

		it('test should be able to remove middle node', function () {
			var rbt = new RedBlackTree();
			rbt.add(new TestNumber(5));
			rbt.add(new TestNumber(10));
			rbt.add(new TestNumber(6));
			rbt.remove(new TestNumber(10));

			assert.equal(5, rbt.min().value, "root should now be 10");
			assert.ok(!rbt.find(new TestNumber(10)));
		});

		it('test should be able to remove last node', function () {
			var rbt = new RedBlackTree();
			rbt.add(new TestNumber(5));
			rbt.add(new TestNumber(10));
			rbt.add(new TestNumber(6));
			rbt.remove(new TestNumber(6));

			assert.equal(5, rbt.min().value, "root should now be 10");
			assert.ok(!rbt.find(new TestNumber(6)));
		});

		it('test should be able to remove all starting with last', function () {
			var rbt = new RedBlackTree();
			rbt.add(new TestNumber(5));
			rbt.add(new TestNumber(10));
			rbt.add(new TestNumber(6));
			rbt.remove(new TestNumber(6));
			rbt.remove(new TestNumber(10));
			rbt.remove(new TestNumber(5));

			assert.ok(!rbt.find(new TestNumber(5)));
			assert.ok(!rbt.find(new TestNumber(6)));
			assert.ok(!rbt.find(new TestNumber(10)));
		});

		it('test should be able to traverse graph', function () {
			var rbt = new RedBlackTree();
			rbt.add(new TestNumber(5));
			rbt.add(new TestNumber(10));
			rbt.add(new TestNumber(6));
			min = rbt.min();
			assert.deepEqual(new TestNumber(6), rbt.findNext(min), "Tree should be traversable.");
		});

		it('test should be able find something', function () {
			var rbt = new RedBlackTree();
			rbt.add(new TestNumber(5));
			rbt.add(new TestNumber(10));
			rbt.add(new TestNumber(6));

			assert.equal(rbt.find(new TestNumber(10)).value, 10, "Tree should be searchable.");
		});
	});
	describe('sweepline', function () {
		it('test can find', function () {
			var geojson = [[100.0, 0.0], [101.0, 0.0], [101.0, 1.0], [100.0, 1.0], [100.0, 0.0]];
			var points = geojson.map(function (pnt) {
				return new Point(pnt[0], pnt[1]);
			});
			var polygon = new Polygon(points);
			var sweep_line = new SweepLine(polygon);
			var event_queue = new EventQueue(polygon);

			while (ev = event_queue.events.pop()) {
				sweep_line.add(ev);
			}

			assert.ok(sweep_line.find({edge: 1}));
		});
	});
});
