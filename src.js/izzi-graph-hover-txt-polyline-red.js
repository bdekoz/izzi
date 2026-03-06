/**
 * izzi-script-graph-hover-txt-polyline-red.js
 * Version 20.2
 * gemini
 */

document.addEventListener("DOMContentLoaded", () => {
    const svgElement = document.querySelector("svg");
    const compositeChart = document.getElementById("composite-chart");

    if (!svgElement || !compositeChart) return;

    // Retrieve all line-graph groups
    const lineGraphs = Array.from(compositeChart.querySelectorAll("g"))
			    .filter(g => g.id && g.id.includes("line-graph"));

    const originalStyles = new Map();
    const text_radius = 5;
    const vec_radius = 15;

    // Store original styling for restoration
    lineGraphs.forEach(g => {
	const texts = Array.from(g.querySelectorAll("text"));
	const polylines = Array.from(g.querySelectorAll("polyline"));
	const markers = Array.from(g.querySelectorAll("circle, path, rect"));

	originalStyles.set(g, {
	    texts: texts.map(t => ({
		el: t,
		fontSize: window.getComputedStyle(t).fontSize,
		fill: t.getAttribute("fill") || window.getComputedStyle(t).fill
	    })),
	    polylines: polylines.map(p => ({
		el: p,
		stroke: p.getAttribute("stroke") || window.getComputedStyle(p).stroke
	    })),
	    markers: markers.map(m => ({
		el: m,
		fillOpacity: m.getAttribute("fill-opacity") || m.style.fillOpacity || window.getComputedStyle(m).fillOpacity || "1",
		fill: m.getAttribute("fill") || window.getComputedStyle(m).fill,
		stroke: m.getAttribute("stroke") || window.getComputedStyle(m).stroke
	    }))
	});
    });

    let activeGraph = null;
    let isResting = false;

    // Math: Distance from a point to a line segment
    function pointToSegmentDistance(px, py, x1, y1, x2, y2) {
	let l2 = (x2 - x1) ** 2 + (y2 - y1) ** 2;
	if (l2 === 0) return Math.hypot(px - x1, py - y1);
	let t = ((px - x1) * (x2 - x1) + (py - y1) * (y2 - y1)) / l2;
	t = Math.max(0, Math.min(1, t));
	return Math.hypot(px - (x1 + t * (x2 - x1)), py - (y1 + t * (y2 - y1)));
    }

    // Calculates pure screen pixel distance from mouse to the actual transformed SVG polyline
    function getScreenDistanceToPolyline(clientX, clientY, polyline) {
	const pts = polyline.points;
	if (!pts || pts.numberOfItems === 0) return Infinity;
	const ctm = polyline.getScreenCTM();
	const pt = svgElement.createSVGPoint();

	let minDist = Infinity;
	for (let i = 0; i < pts.numberOfItems - 1; i++) {
	    let p1 = pts.getItem(i);
	    let p2 = pts.getItem(i + 1);

	    pt.x = p1.x; pt.y = p1.y;
	    let sp1 = pt.matrixTransform(ctm);

	    pt.x = p2.x; pt.y = p2.y;
	    let sp2 = pt.matrixTransform(ctm);

	    let d = pointToSegmentDistance(clientX, clientY, sp1.x, sp1.y, sp2.x, sp2.y);
	    if (d < minDist) minDist = d;
	}
	return minDist;
    }

    function isInTextActivationArea(clientX, clientY, g) {
	const texts = originalStyles.get(g).texts;
	return texts.some(t => {
	    const rect = t.el.getBoundingClientRect();
	    return (clientX >= rect.left - text_radius && clientX <= rect.right + text_radius &&
		    clientY >= rect.top - text_radius && clientY <= rect.bottom + text_radius);
	});
    }

    function isInExtendedActivationArea(clientX, clientY, g) {
	if (isInTextActivationArea(clientX, clientY, g)) return true;

	const polylines = originalStyles.get(g).polylines;
	return polylines.some(p => {
	    return getScreenDistanceToPolyline(clientX, clientY, p.el) <= vec_radius;
	});
    }

    function applyRestStyling() {
	lineGraphs.forEach(g => {
	    const data = originalStyles.get(g);
	    data.markers.forEach(m => {
		m.el.style.fillOpacity = '0';
	    });
	    data.polylines.forEach(p => {
		p.el.style.stroke = 'rgba(128, 128, 128, 0.2)';
	    });
	    data.texts.forEach(t => {
		t.el.style.fontSize = t.fontSize;
		t.el.style.fill = t.fill;
	    });
	});
    }

    function applyActiveStyling(g) {
	const data = originalStyles.get(g);

	data.texts.forEach(t => {
	    let currentSize = parseFloat(t.fontSize) || 12;
	    t.el.style.fontSize = (currentSize * 1.5) + 'px';
	    t.el.style.fill = 'black';
	});

	data.polylines.forEach(p => {
	    p.el.style.stroke = 'red';
	});

	data.markers.forEach(m => {
	    m.el.style.fillOpacity = '1';
	    m.el.style.fill = 'red';
	    m.el.style.stroke = 'red';
	});
    }

    function applyOriginalStyling() {
	lineGraphs.forEach(g => {
	    const data = originalStyles.get(g);

	    data.markers.forEach(m => {
		m.el.style.fillOpacity = m.fillOpacity;

		// Clear inline styles to allow attributes to show, or re-apply original style
		m.el.style.fill = m.fill || '';
		m.el.style.stroke = m.stroke || '';

		if(m.fill) m.el.setAttribute("fill", m.fill);
		if(m.stroke) m.el.setAttribute("stroke", m.stroke);
	    });

	    data.polylines.forEach(p => {
		p.el.style.stroke = p.stroke || '';
		if(p.stroke) p.el.setAttribute("stroke", p.stroke);
	    });

	    data.texts.forEach(t => {
		t.el.style.fontSize = t.fontSize;
		t.el.style.fill = t.fill || '';
		if(t.fill) t.el.setAttribute("fill", t.fill);
	    });
	});
    }

    // Attach events to the Root SVG
    svgElement.addEventListener("mouseenter", () => {
	isResting = true;
	applyRestStyling();
    });

    svgElement.addEventListener("mousemove", (e) => {
	if (!isResting) {
	    isResting = true;
	    applyRestStyling();
	}

	const cx = e.clientX;
	const cy = e.clientY;

	if (activeGraph) {
	    if (isInExtendedActivationArea(cx, cy, activeGraph)) {
		return;
	    } else {
		activeGraph = null;
		applyRestStyling();
	    }
	}

	if (!activeGraph) {
	    for (let g of lineGraphs) {
		if (isInTextActivationArea(cx, cy, g)) {
		    activeGraph = g;
		    applyActiveStyling(g);
		    break;
		}
	    }
	}
    });

    svgElement.addEventListener("mouseleave", () => {
	isResting = false;
	activeGraph = null;
	applyOriginalStyling();
    });
});
