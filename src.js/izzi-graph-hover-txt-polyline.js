// v14 target-on/target-off with extended active area (text + polyline proximity)
(function() {
    // Configuration
    const text_radius = 5;      // Radius around text bounding rect
    const vec_radius = 10;      // Radius around polyline points
    const verbosep = true;      // Enable/disable console logging

    const originalStyles = new Map();
    let isMouseInChart = false;
    let activeLineGraph = null;
    let compositeChart = null;

    // Store polyline points for distance checking
    const polylinePoints = new Map();

    // Helper for conditional logging
    function log(...args) {
	if (verbosep) {
	    console.log(...args);
	}
    }

    function init() {
	compositeChart = document.getElementById('composite-chart');

	if (!compositeChart) {
	    log('composite-chart not found, retrying...');
	    setTimeout(init, 500);
	    return;
	}

	log('composite-chart found, text radius:', text_radius, 'px, vec radius:', vec_radius, 'px');

	// Extract polyline points for distance checking
	extractPolylinePoints();

	// Track mouse position
	document.addEventListener('mousemove', function(e) {
	    handleMouseMove(e);
	});
    }

    function extractPolylinePoints() {
	const lineGraphs = compositeChart.querySelectorAll('[id^="line-graph-"]');

	lineGraphs.forEach(function(lineGraph) {
	    const points = [];
	    const polylineGroup = lineGraph.querySelector('[id^="polyline-"]');

	    if (polylineGroup) {
		const lineElements = polylineGroup.querySelectorAll('path, line, polyline');

		lineElements.forEach(function(element) {
		    if (element.tagName === 'PATH') {
			const pathPoints = extractPathPoints(element);
			points.push(...pathPoints);
		    } else if (element.tagName === 'LINE') {
			const x1 = parseFloat(element.getAttribute('x1'));
			const y1 = parseFloat(element.getAttribute('y1'));
			const x2 = parseFloat(element.getAttribute('x2'));
			const y2 = parseFloat(element.getAttribute('y2'));
			points.push({x: x1, y: y1});
			points.push({x: x2, y: y2});
		    } else if (element.tagName === 'POLYLINE') {
			const pointsAttr = element.getAttribute('points');
			if (pointsAttr) {
			    const pointStrings = pointsAttr.trim().split(/\s+/);
			    pointStrings.forEach(function(pointStr) {
				const [x, y] = pointStr.split(',').map(parseFloat);
				points.push({x, y});
			    });
			}
		    }
		});
	    }

	    polylinePoints.set(lineGraph, points);
	});

	log(`Extracted polyline points for ${lineGraphs.length} line-graphs`);
    }

    function extractPathPoints(pathElement) {
	const points = [];
	const d = pathElement.getAttribute('d');
	if (!d) return points;

	const commands = d.match(/[ML]\s*[-\d\.]+\s*[-\d\.]+/g) || [];

	commands.forEach(function(cmd) {
	    const coords = cmd.match(/[-\d\.]+/g);
	    if (coords && coords.length >= 2) {
		points.push({
		    x: parseFloat(coords[0]),
		    y: parseFloat(coords[1])
		});
	    }
	});

	return points;
    }

    function handleMouseMove(e) {
	// Get chart bounds
	const chartRect = compositeChart.getBoundingClientRect();
	const nowInBounds = e.clientX >= chartRect.left &&
			   e.clientX <= chartRect.right &&
			   e.clientY >= chartRect.top &&
			   e.clientY <= chartRect.bottom;

	// Handle chart state
	if (nowInBounds && !isMouseInChart) {
	    // Mouse entered chart
	    isMouseInChart = true;
	    log('Mouse entered composite-chart');
	    applyRestStyles();
	} else if (!nowInBounds && isMouseInChart) {
	    // Mouse left chart
	    isMouseInChart = false;
	    log('Mouse left composite-chart');
	    revertAllStyles();
	    activeLineGraph = null;
	    return; // Exit early
	}

	// Check extended active area if in chart
	if (nowInBounds) {
	    checkExtendedActiveArea(e.clientX, e.clientY);
	}
    }

    function checkExtendedActiveArea(mouseX, mouseY) {
	const lineGraphs = compositeChart.querySelectorAll('[id^="line-graph-"]');
	let hoveredLineGraph = null;

	// Convert screen coordinates to SVG coordinates for polyline distance check
	let svgCoords = null;
	try {
	    const svg = compositeChart.closest('svg');
	    const point = svg.createSVGPoint();
	    point.x = mouseX;
	    point.y = mouseY;
	    const ctm = svg.getScreenCTM();
	    if (ctm) {
		svgCoords = point.matrixTransform(ctm.inverse());
	    }
	} catch (error) {
	    log('Coordinate conversion error:', error);
	}

	// Check each line-graph for extended active area
	for (const lineGraph of lineGraphs) {
	    // Check text proximity first (with text_radius)
	    const textElement = lineGraph.querySelector('text');
	    if (textElement) {
		const textRect = textElement.getBoundingClientRect();
		const paddedTextRect = {
		    left: textRect.left - text_radius,
		    right: textRect.right + text_radius,
		    top: textRect.top - text_radius,
		    bottom: textRect.bottom + text_radius
		};

		if (mouseX >= paddedTextRect.left && mouseX <= paddedTextRect.right &&
		    mouseY >= paddedTextRect.top && mouseY <= paddedTextRect.bottom) {
		    hoveredLineGraph = lineGraph;
		    break;
		}
	    }

	    // If not near text, check polyline proximity (with vec_radius)
	    if (!hoveredLineGraph && svgCoords && polylinePoints.has(lineGraph)) {
		const points = polylinePoints.get(lineGraph);
		for (const point of points) {
		    const distance = Math.sqrt(
			Math.pow(svgCoords.x - point.x, 2) +
			Math.pow(svgCoords.y - point.y, 2)
		    );

		    if (distance <= vec_radius) {
			hoveredLineGraph = lineGraph;
			break;
		    }
		}
	    }

	    if (hoveredLineGraph) break;
	}

	// Handle state changes
	if (hoveredLineGraph && hoveredLineGraph !== activeLineGraph) {
	    // Mouse entered a line-graph's extended active area
	    log('Mouse entered extended active area for:', hoveredLineGraph.id);

	    if (activeLineGraph) {
		// Revert previously active line-graph to rest style
		revertLineGraphToRestStyle(activeLineGraph);
	    }

	    // Set new active line-graph
	    activeLineGraph = hoveredLineGraph;
	    applyActiveStyle(activeLineGraph);

	} else if (!hoveredLineGraph && activeLineGraph) {
	    // Mouse left the extended active area
	    log('Mouse left extended active area');

	    revertLineGraphToRestStyle(activeLineGraph);
	    activeLineGraph = null;
	}
    }

    function applyRestStyles() {
	// Apply rest style to ALL line-graphs
	const lineGraphs = compositeChart.querySelectorAll('[id^="line-graph-"]');

	lineGraphs.forEach(function(lineGraph) {
	    applyRestStyleToLineGraph(lineGraph);
	});

	log('Applied rest styles to all line-graphs');
    }

    function applyRestStyleToLineGraph(lineGraph) {
	// Save original styles before changing if not already saved
	saveOriginalStyles(lineGraph);

	// Markers - make invisible
	const markersGroup = lineGraph.querySelector('[id^="markers-"]');
	if (markersGroup) {
	    const markerElements = markersGroup.querySelectorAll('circle, rect, ellipse, polygon, path');
	    markerElements.forEach(function(element) {
		element.style.fillOpacity = '0';
	    });
	}

	// Polylines - gray 30%
	const polylineGroup = lineGraph.querySelector('[id^="polyline-"]');
	if (polylineGroup) {
	    const lineElements = polylineGroup.querySelectorAll('path, line, polyline');
	    lineElements.forEach(function(element) {
		element.style.stroke = '#4d4d4d';
	    });
	}

	// Text - ensure it's not in active state
	const textElement = lineGraph.querySelector('text');
	if (textElement) {
	    const originalStyle = originalStyles.get(textElement);
	    if (originalStyle) {
		textElement.style.fontSize = originalStyle.fontSize || '';
		textElement.style.fill = originalStyle.fill || '';
	    }
	}
    }

    function applyActiveStyle(lineGraph) {
	// Save original styles before changing if not already saved
	saveOriginalStyles(lineGraph);

	// Text - apply active styling
	const textElement = lineGraph.querySelector('text');
	if (textElement) {
	    const originalStyle = originalStyles.get(textElement);
	    if (originalStyle) {
		// Apply active styling: 1.5x font size and black color
		const originalSize = parseFloat(originalStyle.fontSize) ||
				    parseFloat(getComputedStyle(textElement).fontSize) || 12;
		textElement.style.fontSize = (originalSize * 1.5) + 'px';
		textElement.style.fill = '#000000';
	    }
	}

	// Markers and polylines - revert to original styling
	const markersGroup = lineGraph.querySelector('[id^="markers-"]');
	if (markersGroup) {
	    const markerElements = markersGroup.querySelectorAll('circle, rect, ellipse, polygon, path');
	    markerElements.forEach(function(element) {
		const originalStyle = originalStyles.get(element);
		if (originalStyle && originalStyle.fillOpacity !== undefined) {
		    element.style.fillOpacity = originalStyle.fillOpacity || '';
		}
	    });
	}

	const polylineGroup = lineGraph.querySelector('[id^="polyline-"]');
	if (polylineGroup) {
	    const lineElements = polylineGroup.querySelectorAll('path, line, polyline');
	    lineElements.forEach(function(element) {
		const originalStyle = originalStyles.get(element);
		if (originalStyle && originalStyle.stroke !== undefined) {
		    element.style.stroke = originalStyle.stroke || '';
		}
	    });
	}
    }

    function revertLineGraphToRestStyle(lineGraph) {
	// Simply apply rest style to this line-graph
	applyRestStyleToLineGraph(lineGraph);
    }

    function saveOriginalStyles(lineGraph) {
	// Save markers original styles
	const markersGroup = lineGraph.querySelector('[id^="markers-"]');
	if (markersGroup) {
	    const markerElements = markersGroup.querySelectorAll('circle, rect, ellipse, polygon, path');
	    markerElements.forEach(function(element) {
		if (!originalStyles.has(element)) {
		    originalStyles.set(element, {
			fillOpacity: element.style.fillOpacity || getComputedStyle(element).fillOpacity
		    });
		}
	    });
	}

	// Save polylines original styles
	const polylineGroup = lineGraph.querySelector('[id^="polyline-"]');
	if (polylineGroup) {
	    const lineElements = polylineGroup.querySelectorAll('path, line, polyline');
	    lineElements.forEach(function(element) {
		if (!originalStyles.has(element)) {
		    originalStyles.set(element, {
			stroke: element.style.stroke || getComputedStyle(element).stroke
		    });
		}
	    });
	}

	// Save text original styles
	const textElement = lineGraph.querySelector('text');
	if (textElement && !originalStyles.has(textElement)) {
	    const computedStyle = getComputedStyle(textElement);
	    originalStyles.set(textElement, {
		fontSize: textElement.style.fontSize || computedStyle.fontSize,
		fill: textElement.style.fill || computedStyle.fill
	    });
	}
    }

    function revertAllStyles() {
	originalStyles.forEach(function(originalStyle, element) {
	    if (originalStyle.fillOpacity !== undefined) {
		element.style.fillOpacity = originalStyle.fillOpacity || '';
	    }
	    if (originalStyle.stroke !== undefined) {
		element.style.stroke = originalStyle.stroke || '';
	    }
	    if (originalStyle.fontSize !== undefined) {
		element.style.fontSize = originalStyle.fontSize || '';
	    }
	    if (originalStyle.fill !== undefined) {
		element.style.fill = originalStyle.fill || '';
	    }
	});

	log('Reverted all styles');
	activeLineGraph = null;
    }

    // Start when DOM is ready
    if (document.readyState === 'loading') {
	document.addEventListener('DOMContentLoaded', init);
    } else {
	init();
    }
})();
