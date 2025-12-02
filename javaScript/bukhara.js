let db = [];

const BukharaCityName = "Bukhara";
const CardContainerId = "activity-cards-container";

const ActivityUrl = "/data/activities.json";

console.log("Attempting to fetch from:", ActivityUrl);

let activityBlock = document.querySelector(".activity_block");
let ActivitySetNum = 0;
const maxItemActivity = 8;

// 1) Fetch corrigé
function initActivities(url) {
    fetch(url)
        .then(res => res.json())
        .then(data => {
            db = data; 
            displayActivities();
            setInterval(displayActivities, 5000);
        })
        .catch(err => console.error(err));
}

initActivities(ActivityUrl);

// 2) Affichage corrigé
function displayActivities() {
    activityBlock.classList.add('fade-out');

    setTimeout(() => {

        // 1. Filtrer par ville
        let filtered = db.filter(item => item.city === BukharaCityName);

        // 2. Découper l'ensemble de 8 éléments
        let start = ActivitySetNum * maxItemActivity;
        let end = start + maxItemActivity;
        let currentActivities = filtered.slice(start, end);

        // 3. Affichage
        activityBlock.innerHTML = currentActivities.map(element => `
            <div class="activity">
                <img src="${element.image}">
                <div class="activity_info">
                    <span>NAME: ${element.name}</span>
                    <span>STYLE: ${element.category}</span>
                    <span>PRICES: ${element.price}</span>
                </div>
            </div>
        `).join('');

        activityBlock.classList.remove('fade-out');

        // 4. Passer au prochain set
        ActivitySetNum++;

        // 5. Si on dépasse le nombre d'activités filtrées → recommencer
        if (start + maxItemActivity >= filtered.length) {
            ActivitySetNum = 0;
        }

    }, 500);
}

// hotel

const HotelsUrl = "/data/hotels.json";
let db_h = [];
let filteredHotels = [];
let currentIndex = 0;

// Fetch hotels data
fetch(HotelsUrl)
    .then(res => res.json())
    .then(data => {
        db_h = data;
        // Filter hotels by current city
        filteredHotels = db_h.filter(hotel => hotel.city === BukharaCityName);
        if (filteredHotels.length > 0) {
            displayHotel(currentIndex);
        }
    })
    .catch(err => console.error(err));

// Function to display hotel
function displayHotel(index) {
    if (filteredHotels.length === 0) return;
    
    const hotel = filteredHotels[index];
    const hotelDiv = document.querySelector('.hotel');
    const img = hotelDiv.querySelector('img');
    const span = hotelDiv.querySelector('span');
    
    // Update image (use placeholder if empty)
    img.src = hotel.image || '../images/tashkent.jpg';
    img.alt = hotel.name;
    
    // Update hotel name
    span.textContent = hotel.name;
}

// Left button - previous hotel
document.querySelector('.main_three_left').addEventListener('click', () => {
    if (filteredHotels.length === 0) return;
    
    currentIndex--;
    if (currentIndex < 0) {
        currentIndex = filteredHotels.length - 1; // Loop to last hotel
    }
    displayHotel(currentIndex);
});

// Right button - next hotel
document.querySelector('.main_three_right').addEventListener('click', () => {
    if (filteredHotels.length === 0) return;
    
    currentIndex++;
    if (currentIndex >= filteredHotels.length) {
        currentIndex = 0; // Loop back to first hotel
    }
    displayHotel(currentIndex);
});