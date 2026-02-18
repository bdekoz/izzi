// v13d target-on/target-off with text proximity detection and corrected active styling
(function() {
    // Configuration
    const text_radius = 5; // Configurable radius around text bounding rect
    const verbosep = true; // Enable/disable console logging
    
    const originalStyles = new Map();
    let isMouseInChart = false;
    let activeLineGraph = null;
    let compositeChart = null;
    
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
        
        log('composite-chart found, text radius:', text_radius, 'px');
        
        // Track mouse position
        document.addEventListener('mousemove', function(e) {
            handleMouseMove(e);
        });
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
        
        // Check text proximity if in chart
        if (nowInBounds) {
            checkTextProximity(e.clientX, e.clientY);
        }
    }
    
    function checkTextProximity(mouseX, mouseY) {
        const lineGraphs = compositeChart.querySelectorAll('[id^="line-graph-"]');
        let hoveredLineGraph = null;
        
        // Check each line-graph's text element
        for (const lineGraph of lineGraphs) {
            const textElement = lineGraph.querySelector('text');
            if (!textElement) continue;
            
            // Get text bounding rect with padding
            const textRect = textElement.getBoundingClientRect();
            const paddedRect = {
                left: textRect.left - text_radius,
                right: textRect.right + text_radius,
                top: textRect.top - text_radius,
                bottom: textRect.bottom + text_radius
            };
            
            // Check if mouse is near text
            if (mouseX >= paddedRect.left && mouseX <= paddedRect.right &&
                mouseY >= paddedRect.top && mouseY <= paddedRect.bottom) {
                hoveredLineGraph = lineGraph;
                break;
            }
        }
        
        // Handle state changes
        if (hoveredLineGraph && hoveredLineGraph !== activeLineGraph) {
            // Mouse entered a line-graph's text proximity
            log('Mouse near line-graph text:', hoveredLineGraph.id);
            
            if (activeLineGraph) {
                // Revert previously active line-graph to rest style
                revertLineGraphToRestStyle(activeLineGraph);
            }
            
            // Set new active line-graph
            activeLineGraph = hoveredLineGraph;
            applyActiveStyle(activeLineGraph);
            
        } else if (!hoveredLineGraph && activeLineGraph) {
            // Mouse moved away from active line-graph text
            log('Mouse away from line-graph text');
            
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

