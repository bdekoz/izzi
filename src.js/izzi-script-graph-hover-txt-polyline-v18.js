// izzi-script-graph-hover-txt-polyline.js
// Version 18 - Fixed Polyline Buffer with Debug
(function() {
    // ==================== CONFIGURATION ====================
    const text_radius = 5;      // px around text bounding box for activation
    const vec_radius = 30;      // px perpendicular distance from polyline for activation
    const verbosep = true;      // console logging on/off
    const debug_visuals = true; // Show activation areas (red for polyline, blue for text)

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
    let svgElement = null;
    const polylineSegments = new Map(); // Store line segments in SVG coordinates
    const textBounds = new Map(); // Store text bounds in SVG coordinates

    // Debug elements
    const debugCircles = [];

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
        
        svgElement = compositeChart.closest('svg');
        if (!svgElement) {
            log('SVG element not found');
            return;
        }
        
        log('composite-chart found. text_radius:', text_radius, 'vec_radius:', vec_radius);
        
        // Extract geometry in SVG coordinates
        extractPolylineSegments();
        extractTextBounds();
        
        // Create debug visuals if enabled
        if (debug_visuals) {
            createDebugVisuals();
        }
        
        document.addEventListener('mousemove', handleMouseMove);
        log('Initialization complete');
    }

    function extractPolylineSegments() {
        const lineGraphs = compositeChart.querySelectorAll('[id^="line-graph-"]');
        log(`Found ${lineGraphs.length} line-graphs`);
        
        lineGraphs.forEach((lineGraph, index) => {
            const segments = [];
            const polylineGroup = lineGraph.querySelector('[id^="polyline-"]');
            if (!polylineGroup) {
                log(`No polyline group for graph ${index}`);
                return;
            }

            const lineElements = polylineGroup.querySelectorAll('path, line, polyline');
            log(`Graph ${index} has ${lineElements.length} line elements`);
            
            lineElements.forEach(el => {
                if (el.tagName === 'LINE') {
                    segments.push({
                        x1: parseFloat(el.getAttribute('x1')),
                        y1: parseFloat(el.getAttribute('y1')),
                        x2: parseFloat(el.getAttribute('x2')),
                        y2: parseFloat(el.getAttribute('y2'))
                    });
                } else if (el.tagName === 'POLYLINE' && el.getAttribute('points')) {
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
                    // Simple M/L command parser
                    const points = [];
                    const commands = el.getAttribute('d').match(/[ML]\s*[-\d\.]+\s*[-\d\.]+/g);
                    if (commands) {
                        commands.forEach(cmd => {
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
                }
            });
            
            polylineSegments.set(lineGraph, segments);
            log(`Graph ${lineGraph.id} has ${segments.length} segments`);
        });
    }

    function extractTextBounds() {
        const lineGraphs = compositeChart.querySelectorAll('[id^="line-graph-"]');
        
        lineGraphs.forEach(lineGraph => {
            const textEl = lineGraph.querySelector('text');
            if (textEl) {
                // Get text position from SVG attributes
                const x = parseFloat(textEl.getAttribute('x'));
                const y = parseFloat(textEl.getAttribute('y'));
                if (!isNaN(x) && !isNaN(y)) {
                    textBounds.set(lineGraph, { x, y });
                    log(`Text for ${lineGraph.id} at SVG coords:`, x, y);
                }
            }
        });
    }

    function createDebugVisuals() {
        // Remove old debug circles
        debugCircles.forEach(c => c.remove());
        debugCircles.length = 0;
        
        // Create circles at polyline segment midpoints for debugging
        polylineSegments.forEach((segments, lineGraph) => {
            segments.forEach(seg => {
                // Midpoint of segment
                const mx = (seg.x1 + seg.x2) / 2;
                const my = (seg.y1 + seg.y2) / 2;
                
                const circle = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
                circle.setAttribute('cx', mx);
                circle.setAttribute('cy', my);
                circle.setAttribute('r', vec_radius);
                circle.setAttribute('fill', 'red');
                circle.setAttribute('opacity', '0.1');
                circle.setAttribute('stroke', 'red');
                circle.setAttribute('stroke-width', '1');
                circle.style.pointerEvents = 'none';
                svgElement.appendChild(circle);
                debugCircles.push(circle);
            });
            
            // Add text position indicator
            const textPos = textBounds.get(lineGraph);
            if (textPos) {
                const textCircle = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
                textCircle.setAttribute('cx', textPos.x);
                textCircle.setAttribute('cy', textPos.y);
                textCircle.setAttribute('r', text_radius);
                textCircle.setAttribute('fill', 'blue');
                textCircle.setAttribute('opacity', '0.1');
                textCircle.setAttribute('stroke', 'blue');
                textCircle.setAttribute('stroke-width', '1');
                textCircle.style.pointerEvents = 'none';
                svgElement.appendChild(textCircle);
                debugCircles.push(textCircle);
            }
        });
        
        log('Created debug visuals');
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
        // Get chart bounds (screen coordinates)
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
        // Convert mouse to SVG coordinates
        let svgCoords = null;
        try {
            const ctm = svgElement.getScreenCTM();
            if (ctm) {
                let pt = svgElement.createSVGPoint();
                pt.x = e.clientX;
                pt.y = e.clientY;
                svgCoords = pt.matrixTransform(ctm.inverse());
                
                if (verbosep && Math.random() < 0.01) { // Log occasionally
                    log('Mouse SVG coords:', svgCoords.x.toFixed(2), svgCoords.y.toFixed(2));
                }
            }
        } catch (err) { 
            log('Coordinate conversion error:', err);
            return;
        }
        
        if (!svgCoords) return;
        
        const lineGraphs = compositeChart.querySelectorAll('[id^="line-graph-"]');
        let foundActiveGraph = null;
        
        // First, check if current active graph's extended area still contains mouse
        if (activeLineGraph) {
            if (isPointInExtendedArea(svgCoords, activeLineGraph)) {
                // Still in active graph's area - keep it active
                if (verbosep && Math.random() < 0.01) {
                    log('Still in extended area for:', activeLineGraph.id);
                }
                return;
            } else if (verbosep && Math.random() < 0.01) {
                log('Left active graph area:', activeLineGraph.id);
            }
        }
        
        // If not in current active graph's area, check all graphs to find which one we're in
        for (const lineGraph of lineGraphs) {
            if (isPointInExtendedArea(svgCoords, lineGraph)) {
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
    
    function isPointInExtendedArea(svgCoords, lineGraph) {
        // Check Text Activation Area (using stored SVG text position)
        const textPos = textBounds.get(lineGraph);
        if (textPos) {
            const dist = Math.hypot(svgCoords.x - textPos.x, svgCoords.y - textPos.y);
            if (dist <= text_radius) {
                return true;
            }
        }
        
        // Check Polyline Activation Area (distance to any line segment)
        const segments = polylineSegments.get(lineGraph);
        if (segments && segments.length > 0) {
            for (const seg of segments) {
                const dist = distanceToSegment(
                    svgCoords.x, svgCoords.y,
                    seg.x1, seg.y1, seg.x2, seg.y2
                );
                if (dist <= vec_radius) {
                    return true;
                }
            }
        }
        
        return false;
    }

    // --- Styling Functions (unchanged) ---
    function saveOriginalStyles(lineGraph) {
        const markersGroup = lineGraph.querySelector('[id^="markers-"]');
        markersGroup?.querySelectorAll('circle, rect, ellipse, polygon, path').forEach(el => {
            if (!originalStyles.has(el)) {
                originalStyles.set(el, { 
                    fill: el.style.fill || getComputedStyle(el).fill, 
                    fillOpacity: el.style.fillOpacity || getComputedStyle(el).fillOpacity 
                });
            }
        });
        
        const polylineGroup = lineGraph.querySelector('[id^="polyline-"]');
        polylineGroup?.querySelectorAll('path, line, polyline').forEach(el => {
            if (!originalStyles.has(el)) {
                originalStyles.set(el, { 
                    stroke: el.style.stroke || getComputedStyle(el).stroke 
                });
            }
        });
        
        const textEl = lineGraph.querySelector('text');
        if (textEl && !originalStyles.has(textEl)) {
            let cs = getComputedStyle(textEl);
            originalStyles.set(textEl, { 
                fontSize: textEl.style.fontSize || cs.fontSize, 
                fill: textEl.style.fill || cs.fill 
            });
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
                if (orig) { 
                    textEl.style.fontSize = orig.fontSize || ''; 
                    textEl.style.fill = orig.fill || ''; 
                }
            }
        });
        log('Applied rest styles.');
    }

    function applyActiveStyle(lineGraph) {
        saveOriginalStyles(lineGraph);
        log('Applying ACTIVE style to', lineGraph.id);

        let textEl = lineGraph.querySelector('text');
        if (textEl) {
            let orig = originalStyles.get(textEl);
            let baseSize = parseFloat(orig?.fontSize) || parseFloat(getComputedStyle(textEl).fontSize) || 12;
            textEl.style.fontSize = (baseSize * ACTIVE_STYLE.textSizeMultiplier) + 'px';
            textEl.style.fill = ACTIVE_STYLE.textColor;
        }

        const markersGroup = lineGraph.querySelector('[id^="markers-"]');
        markersGroup?.querySelectorAll('circle, rect, ellipse, polygon, path').forEach(el => {
            let orig = originalStyles.get(el);
            if (orig?.fill !== undefined) el.style.fill = orig.fill || '';
            el.style.fill = ACTIVE_STYLE.markerFill;
            if (orig?.fillOpacity !== undefined) el.style.fillOpacity = orig.fillOpacity || '';
        });

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
            if (orig) { 
                textEl.style.fontSize = orig.fontSize || ''; 
                textEl.style.fill = orig.fill || ''; 
            }
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

