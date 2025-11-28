let db = [];

const SamarkandCityName = "Samarkand";
const CardContainerId = "activity-cards-container";

const ActivityUrl = "http://127.0.0.1:5501/cpp-project/data/activities.json";

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
        let filtered = db.filter(item => item.city === "Samarkand");

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
