
// izzi-script-graph-hover-txt-polyline.js - Version 15
(function() {
    // ==================== CONFIGURATION ====================
    const text_radius = 5;      // px around text bounding box for activation
    const vec_radius = 10;      // px around polyline points for extended activation
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
    const polylinePoints = new Map(); // Store points for vec_radius check

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
        extractPolylinePoints();
        document.addEventListener('mousemove', handleMouseMove);
    }

    function extractPolylinePoints() {
        const lineGraphs = compositeChart.querySelectorAll('[id^="line-graph-"]');
        lineGraphs.forEach(lineGraph => {
            const points = [];
            const polylineGroup = lineGraph.querySelector('[id^="polyline-"]');
            if (!polylineGroup) return;

            polylineGroup.querySelectorAll('path, line, polyline').forEach(el => {
                if (el.tagName === 'LINE') {
                    points.push({ x: parseFloat(el.getAttribute('x1')), y: parseFloat(el.getAttribute('y1')) });
                    points.push({ x: parseFloat(el.getAttribute('x2')), y: parseFloat(el.getAttribute('y2')) });
                } else if (el.tagName === 'POLYLINE' && el.getAttribute('points')) {
                    el.getAttribute('points').trim().split(/\s+/).forEach(p => {
                        let [x, y] = p.split(',').map(parseFloat);
                        points.push({ x, y });
                    });
                } else if (el.tagName === 'PATH' && el.getAttribute('d')) {
                    // Simple M/L command parser for demonstration
                    el.getAttribute('d').match(/[ML]\s*[-\d\.]+\s*[-\d\.]+/g)?.forEach(cmd => {
                        let coords = cmd.match(/[-\d\.]+/g);
                        if (coords?.length >= 2) points.push({ x: parseFloat(coords[0]), y: parseFloat(coords[1]) });
                    });
                }
            });
            polylinePoints.set(lineGraph, points);
        });
        log(`Extracted points for ${lineGraphs.length} line-graphs.`);
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
        let newlyActivatedGraph = null;

        // 1. Convert mouse to SVG coordinates for polyline distance check
        let svgCoords = null;
        try {
            const svg = compositeChart.closest('svg');
            const ctm = svg.getScreenCTM();
            if (ctm) {
                let pt = svg.createSVGPoint(); pt.x = e.clientX; pt.y = e.clientY;
                svgCoords = pt.matrixTransform(ctm.inverse());
            }
        } catch (err) { log('SVG coord conversion error:', err); }

        // 2. Check each line-graph
        for (const lineGraph of lineGraphs) {
            // --- Check Text Activation Area ---
            const textEl = lineGraph.querySelector('text');
            if (textEl) {
                const textRect = textEl.getBoundingClientRect();
                if (e.clientX >= textRect.left - text_radius && e.clientX <= textRect.right + text_radius &&
                    e.clientY >= textRect.top - text_radius && e.clientY <= textRect.bottom + text_radius) {
                    newlyActivatedGraph = lineGraph;
                    log('Activation via TEXT for', lineGraph.id);
                    break;
                }
            }

            // --- Check Polyline Activation Area (if not already activated) ---
            if (!newlyActivatedGraph && svgCoords && polylinePoints.has(lineGraph)) {
                for (const point of polylinePoints.get(lineGraph)) {
                    const dist = Math.hypot(svgCoords.x - point.x, svgCoords.y - point.y);
                    if (dist <= vec_radius) {
                        newlyActivatedGraph = lineGraph;
                        log('Activation via POLYLINE for', lineGraph.id, 'distance:', dist.toFixed(2));
                        break;
                    }
                }
            }
            if (newlyActivatedGraph) break;
        }

        // 3. Handle State Changes
        if (newlyActivatedGraph && newlyActivatedGraph !== activeLineGraph) {
            // New graph activated
            if (activeLineGraph) revertLineGraphToRest(activeLineGraph);
            activeLineGraph = newlyActivatedGraph;
            applyActiveStyle(activeLineGraph);
        } else if (!newlyActivatedGraph && activeLineGraph) {
            // Left active area
            log('Left extended activation area for', activeLineGraph.id);
            revertLineGraphToRest(activeLineGraph);
            activeLineGraph = null;
        }
    }

    // --- Styling Functions ---
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
            if (lg === activeLineGraph) return; // Don't restyle active graph here
            saveOriginalStyles(lg);

            lg.querySelector('[id^="markers-"]')?.querySelectorAll('circle, rect, ellipse, polygon, path').forEach(el => {
                el.style.fillOpacity = REST_STYLE.markerFillOpacity;
            });
            lg.querySelector('[id^="polyline-"]')?.querySelectorAll('path, line, polyline').forEach(el => {
                el.style.stroke = REST_STYLE.polylineStroke;
            });
            // Ensure text is original
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

        // Style Markers & Polylines (revert to original, then apply red)
        // Markers: restore original, then set fill to red
        const markersGroup = lineGraph.querySelector('[id^="markers-"]');
        markersGroup?.querySelectorAll('circle, rect, ellipse, polygon, path').forEach(el => {
            let orig = originalStyles.get(el);
            if (orig?.fill !== undefined) el.style.fill = orig.fill || ''; // restore original
            el.style.fill = ACTIVE_STYLE.markerFill; // override with red
            if (orig?.fillOpacity !== undefined) el.style.fillOpacity = orig.fillOpacity || ''; // restore opacity
        });

        // Polylines: restore original, then set stroke to red
        const polylineGroup = lineGraph.querySelector('[id^="polyline-"]');
        polylineGroup?.querySelectorAll('path, line, polyline').forEach(el => {
            let orig = originalStyles.get(el);
            if (orig?.stroke !== undefined) el.style.stroke = orig.stroke || ''; // restore original
            el.style.stroke = ACTIVE_STYLE.polylineStroke; // override with red
        });
    }

    function revertLineGraphToRest(lineGraph) {
        log('Reverting', lineGraph.id, 'to rest style.');
        // Re-apply rest styles to this specific graph
        saveOriginalStyles(lineGraph); // Ensure original are saved before we potentially mess with them

        // Markers to invisible
        lineGraph.querySelector('[id^="markers-"]')?.querySelectorAll('circle, rect, ellipse, polygon, path').forEach(el => {
            el.style.fillOpacity = REST_STYLE.markerFillOpacity;
        });
        // Polylines to gray
        lineGraph.querySelector('[id^="polyline-"]')?.querySelectorAll('path, line, polyline').forEach(el => {
            el.style.stroke = REST_STYLE.polylineStroke;
        });
        // Text to original
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

