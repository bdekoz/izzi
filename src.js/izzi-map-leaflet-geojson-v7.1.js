// baseline-v7.1 - Function-generated geojson map
// deepthink 20260329
function leaflet_map_geojson(geojsonUrl) {
    // Initialize map
    const map = L.map('map').setView([20, 0], 2);
    
    L.tileLayer('https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png', {
        attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> &copy; CartoDB',
        subdomains: 'abcd',
        maxZoom: 19
    }).addTo(map);
    
    // Store markers for clustering
    let markers = [];
    
    // Helper function to format numbers
    function formatNumber(num) {
        return num.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
    }
    
    // Load GeoJSON data
    fetch(geojsonUrl)
        .then(response => response.json())
        .then(data => {
            // Process each feature - properties are already objects, no need to parse
            data.features.forEach(feature => {
                const props = feature.properties;
                const coords = feature.geometry.coordinates;
                
                // Get values directly from properties
                const downloaders = props.downloaders?.size || 0;
                const uploaders = props.uploaders?.size || 0;
                const totalPeers = downloaders + uploaders;
                
                if (totalPeers === 0) return;
                
                // Create circle marker
                const radius = Math.sqrt(totalPeers) * 0.2 + 5;
                const circle = L.circleMarker([coords[1], coords[0]], {
                    radius: radius,
                    fillColor: '#2c7bb6',
                    color: '#333',
                    weight: 1,
                    opacity: 0.8,
                    fillOpacity: 0.7
                });
                
                // Create popup content with direct property access
                const popupContent = `
                    <div style="min-width: 200px;">
                        <h3>${props.city || 'Unknown'}, ${props.country_code || 'Unknown'}</h3>
                        <p><strong>Downloaders:</strong> ${formatNumber(downloaders)}</p>
                        <p><strong>Uploaders:</strong> ${formatNumber(uploaders)}</p>
                        <p><strong>Total Peers:</strong> ${formatNumber(totalPeers)}</p>
                        <hr>
                        <p><strong>Downloader Breakdown:</strong></p>
                        <ul>
                            <li>Mobile: ${formatNumber(props.downloaders?.mobile || 0)}</li>
                            <li>Satellite: ${formatNumber(props.downloaders?.satellite || 0)}</li>
                            <li>VPN: ${formatNumber(props.downloaders?.vpn || 0)}</li>
                            <li>Hosting: ${formatNumber(props.downloaders?.hosting || 0)}</li>
                            <li>Service: ${formatNumber(props.downloaders?.service || 0)}</li>
                        </ul>
                        <p><strong>Uploader Breakdown:</strong></p>
                        <ul>
                            <li>Mobile: ${formatNumber(props.uploaders?.mobile || 0)}</li>
                            <li>Satellite: ${formatNumber(props.uploaders?.satellite || 0)}</li>
                            <li>VPN: ${formatNumber(props.uploaders?.vpn || 0)}</li>
                            <li>Hosting: ${formatNumber(props.uploaders?.hosting || 0)}</li>
                            <li>Service: ${formatNumber(props.uploaders?.service || 0)}</li>
                        </ul>
                    </div>
                `;
                
                circle.bindPopup(popupContent);
                markers.push(circle);
            });
            
            // Add markers to map with clustering
            const clusterGroup = L.markerClusterGroup();
            clusterGroup.addLayers(markers);
            map.addLayer(clusterGroup);
            
            // Fit bounds to show all markers
            if (markers.length > 0) {
                const group = L.featureGroup(markers);
                map.fitBounds(group.getBounds().pad(0.1));
            }
        })
        .catch(error => {
            console.error('Error loading GeoJSON:', error);
        });
    
    return map;
}
