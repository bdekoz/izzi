// izzi-script-graph-hover-txt-polyline.js
// Version 17 - True Polyline Buffer Zone
(function() {
    // ==================== CONFIGURATION ====================
    const text_radius = 5;      // px around text bounding box for activation
    const vec_radius = 10;      // px perpendicular distance from polyline for activation
    const verbosep = true;      // console logging on/off

    // Style Constants
    const REST_STYLE = {
        markerFillOpacity: '0',
        polylineStroke: '#333333' // gray 20% (approx #333)
    };
    const ACTIVE_STYLE = {
        textColor: '#000000',
        textSizeMultiplier: 1.5,
        markerFill: '#ff0000',    // red
        polylineStroke: '#ff0000' // red
    };
    // =======================================================

    const originalStyles = new Map();
    let isMouseInChart = false;
    let activeLineGraph = null;
    let compositeChart = null;
    const polylineSegments = new Map(); // Store line segments for distance calculation

    // --- Utility Functions ---
    function log(...args) { if (verbosep) console.log(...args); }

    // --- Initialization ---
    function init() {
        compositeChart = document.getElementById('composite-chart');
        if (!compositeChart) {
            log('composite-chart not found, retrying...');
            setTimeout(init, 500);
            return;
        }
        log('composite-chart found. text_radius:', text_radius, 'vec_radius:', vec_radius);
        extractPolylineSegments();
        document.addEventListener('mousemove', handleMouseMove);
    }

    function extractPolylineSegments() {
        const lineGraphs = compositeChart.querySelectorAll('[id^="line-graph-"]');
        lineGraphs.forEach(lineGraph => {
            const segments = [];
            const polylineGroup = lineGraph.querySelector('[id^="polyline-"]');
            if (!polylineGroup) return;

            polylineGroup.querySelectorAll('path, line, polyline').forEach(el => {
                if (el.tagName === 'LINE') {
                    // Single line segment
                    segments.push({
                        x1: parseFloat(el.getAttribute('x1')),
                        y1: parseFloat(el.getAttribute('y1')),
                        x2: parseFloat(el.getAttribute('x2')),
                        y2: parseFloat(el.getAttribute('y2'))
                    });
                } else if (el.tagName === 'POLYLINE' && el.getAttribute('points')) {
                    // Convert polyline points to line segments
                    const points = [];
                    el.getAttribute('points').trim().split(/\s+/).forEach(p => {
                        let [x, y] = p.split(',').map(parseFloat);
                        points.push({ x, y });
                    });
                    
                    for (let i = 0; i < points.length - 1; i++) {
                        segments.push({
                            x1: points[i].x,
                            y1: points[i].y,
                            x2: points[i+1].x,
                            y2: points[i+1].y
                        });
                    }
                } else if (el.tagName === 'PATH' && el.getAttribute('d')) {
                    // Simple M/L command parser - convert to segments
                    const points = [];
                    el.getAttribute('d').match(/[ML]\s*[-\d\.]+\s*[-\d\.]+/g)?.forEach(cmd => {
                        let coords = cmd.match(/[-\d\.]+/g);
                        if (coords?.length >= 2) {
                            points.push({ x: parseFloat(coords[0]), y: parseFloat(coords[1]) });
                        }
                    });
                    
                    for (let i = 0; i < points.length - 1; i++) {
                        segments.push({
                            x1: points[i].x,
                            y1: points[i].y,
                            x2: points[i+1].x,
                            y2: points[i+1].y
                        });
                    }
                }
            });
            polylineSegments.set(lineGraph, segments);
        });
        log(`Extracted polyline segments for ${lineGraphs.length} line-graphs.`);
    }

    // Distance from point to line segment
    function distanceToSegment(px, py, x1, y1, x2, y2) {
        const A = px - x1;
        const B = py - y1;
        const C = x2 - x1;
        const D = y2 - y1;
        
        const dot = A * C + B * D;
        const lenSq = C * C + D * D;
        let param = -1;
        
        if (lenSq !== 0) {
            param = dot / lenSq;
        }
        
        let xx, yy;
        
        if (param < 0) {
            xx = x1;
            yy = y1;
        } else if (param > 1) {
            xx = x2;
            yy = y2;
        } else {
            xx = x1 + param * C;
            yy = y1 + param * D;
        }
        
        const dx = px - xx;
        const dy = py - yy;
        return Math.sqrt(dx * dx + dy * dy);
    }

    // --- Mouse Tracking & State Management ---
    function handleMouseMove(e) {
        const chartRect = compositeChart.getBoundingClientRect();
        const nowInChart = e.clientX >= chartRect.left && e.clientX <= chartRect.right &&
                           e.clientY >= chartRect.top && e.clientY <= chartRect.bottom;

        // Chart Entry / Exit
        if (nowInChart && !isMouseInChart) {
            isMouseInChart = true;
            log('Mouse ENTERED composite-chart.');
            applyRestStyles();
        } else if (!nowInChart && isMouseInChart) {
            isMouseInChart = false;
            log('Mouse LEFT composite-chart.');
            revertAllStyles();
            activeLineGraph = null;
            return;
        }

        // Check activation areas if inside chart
        if (nowInChart) {
            checkExtendedActivation(e);
        }
    }

    function checkExtendedActivation(e) {
        const lineGraphs = compositeChart.querySelectorAll('[id^="line-graph-"]');
        let foundActiveGraph = null;
        
        // First, check if current active graph's extended area still contains mouse
        if (activeLineGraph) {
            if (isPointInExtendedArea(e, activeLineGraph)) {
                // Still in active graph's area - keep it active
                log('Still in extended area for:', activeLineGraph.id);
                return;
            }
        }
        
        // If not in current active graph's area, check all graphs to find which one we're in
        for (const lineGraph of lineGraphs) {
            if (isPointInExtendedArea(e, lineGraph)) {
                foundActiveGraph = lineGraph;
                break;
            }
        }

        // Handle state changes
        if (foundActiveGraph && foundActiveGraph !== activeLineGraph) {
            // New graph activated
            log('Switching active graph to:', foundActiveGraph.id);
            if (activeLineGraph) {
                revertLineGraphToRest(activeLineGraph);
            }
            activeLineGraph = foundActiveGraph;
            applyActiveStyle(activeLineGraph);
        } else if (!foundActiveGraph && activeLineGraph) {
            // Left all extended areas
            log('Left extended activation area for', activeLineGraph.id);
            revertLineGraphToRest(activeLineGraph);
            activeLineGraph = null;
        }
    }
    
    function isPointInExtendedArea(e, lineGraph) {
        // Check Text Activation Area
        const textEl = lineGraph.querySelector('text');
        if (textEl) {
            const textRect = textEl.getBoundingClientRect();
            if (e.clientX >= textRect.left - text_radius && e.clientX <= textRect.right + text_radius &&
                e.clientY >= textRect.top - text_radius && e.clientY <= textRect.bottom + text_radius) {
                return true;
            }
        }
        
        // Check Polyline Activation Area (distance to any line segment)
        try {
            const svg = compositeChart.closest('svg');
            const ctm = svg.getScreenCTM();
            if (ctm && polylineSegments.has(lineGraph)) {
                let pt = svg.createSVGPoint(); pt.x = e.clientX; pt.y = e.clientY;
                const svgCoords = pt.matrixTransform(ctm.inverse());
                
                // Check distance to each line segment
                for (const seg of polylineSegments.get(lineGraph)) {
                    const dist = distanceToSegment(
                        svgCoords.x, svgCoords.y,
                        seg.x1, seg.y1, seg.x2, seg.y2
                    );
                    if (dist <= vec_radius) {
                        return true;
                    }
                }
            }
        } catch (err) { 
            // Silent fail for coord conversion
        }
        
        return false;
    }

    // --- Styling Functions (unchanged from previous version) ---
    function saveOriginalStyles(lineGraph) {
        // Markers
        const markersGroup = lineGraph.querySelector('[id^="markers-"]');
        markersGroup?.querySelectorAll('circle, rect, ellipse, polygon, path').forEach(el => {
            if (!originalStyles.has(el)) {
                originalStyles.set(el, { fill: el.style.fill || getComputedStyle(el).fill, fillOpacity: el.style.fillOpacity || getComputedStyle(el).fillOpacity });
            }
        });
        // Polylines
        const polylineGroup = lineGraph.querySelector('[id^="polyline-"]');
        polylineGroup?.querySelectorAll('path, line, polyline').forEach(el => {
            if (!originalStyles.has(el)) {
                originalStyles.set(el, { stroke: el.style.stroke || getComputedStyle(el).stroke });
            }
        });
        // Text
        const textEl = lineGraph.querySelector('text');
        if (textEl && !originalStyles.has(textEl)) {
            let cs = getComputedStyle(textEl);
            originalStyles.set(textEl, { fontSize: textEl.style.fontSize || cs.fontSize, fill: textEl.style.fill || cs.fill });
        }
    }

    function applyRestStyles() {
        compositeChart.querySelectorAll('[id^="line-graph-"]').forEach(lg => {
            if (lg === activeLineGraph) return;
            saveOriginalStyles(lg);

            lg.querySelector('[id^="markers-"]')?.querySelectorAll('circle, rect, ellipse, polygon, path').forEach(el => {
                el.style.fillOpacity = REST_STYLE.markerFillOpacity;
            });
            lg.querySelector('[id^="polyline-"]')?.querySelectorAll('path, line, polyline').forEach(el => {
                el.style.stroke = REST_STYLE.polylineStroke;
            });
            let textEl = lg.querySelector('text');
            if (textEl) {
                let orig = originalStyles.get(textEl);
                if (orig) { textEl.style.fontSize = orig.fontSize || ''; textEl.style.fill = orig.fill || ''; }
            }
        });
        log('Applied rest styles.');
    }

    function applyActiveStyle(lineGraph) {
        saveOriginalStyles(lineGraph);
        log('Applying ACTIVE style to', lineGraph.id);

        // Style Text
        let textEl = lineGraph.querySelector('text');
        if (textEl) {
            let orig = originalStyles.get(textEl);
            let baseSize = parseFloat(orig?.fontSize) || parseFloat(getComputedStyle(textEl).fontSize) || 12;
            textEl.style.fontSize = (baseSize * ACTIVE_STYLE.textSizeMultiplier) + 'px';
            textEl.style.fill = ACTIVE_STYLE.textColor;
        }

        // Style Markers
        const markersGroup = lineGraph.querySelector('[id^="markers-"]');
        markersGroup?.querySelectorAll('circle, rect, ellipse, polygon, path').forEach(el => {
            let orig = originalStyles.get(el);
            if (orig?.fill !== undefined) el.style.fill = orig.fill || '';
            el.style.fill = ACTIVE_STYLE.markerFill;
            if (orig?.fillOpacity !== undefined) el.style.fillOpacity = orig.fillOpacity || '';
        });

        // Style Polylines
        const polylineGroup = lineGraph.querySelector('[id^="polyline-"]');
        polylineGroup?.querySelectorAll('path, line, polyline').forEach(el => {
            let orig = originalStyles.get(el);
            if (orig?.stroke !== undefined) el.style.stroke = orig.stroke || '';
            el.style.stroke = ACTIVE_STYLE.polylineStroke;
        });
    }

    function revertLineGraphToRest(lineGraph) {
        log('Reverting', lineGraph.id, 'to rest style.');
        saveOriginalStyles(lineGraph);

        lineGraph.querySelector('[id^="markers-"]')?.querySelectorAll('circle, rect, ellipse, polygon, path').forEach(el => {
            el.style.fillOpacity = REST_STYLE.markerFillOpacity;
        });
        lineGraph.querySelector('[id^="polyline-"]')?.querySelectorAll('path, line, polyline').forEach(el => {
            el.style.stroke = REST_STYLE.polylineStroke;
        });
        let textEl = lineGraph.querySelector('text');
        if (textEl) {
            let orig = originalStyles.get(textEl);
            if (orig) { textEl.style.fontSize = orig.fontSize || ''; textEl.style.fill = orig.fill || ''; }
        }
    }

    function revertAllStyles() {
        originalStyles.forEach((style, el) => {
            if (style.fill !== undefined) el.style.fill = style.fill || '';
            if (style.fillOpacity !== undefined) el.style.fillOpacity = style.fillOpacity || '';
            if (style.stroke !== undefined) el.style.stroke = style.stroke || '';
            if (style.fontSize !== undefined) el.style.fontSize = style.fontSize || '';
        });
        log('Reverted all styles to original.');
        activeLineGraph = null;
    }

    // --- Start ---
    if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', init);
    else init();
})();

