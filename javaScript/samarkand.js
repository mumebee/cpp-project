let activity = [
    {
        "name": "Safari",
        "style": "active",
        "price": "from 10$ and more"
    },
    {
        "name": "Trampoline",
        "style": "active",
        "price": "from 20$ and more"
    },
    {
        "name": "Magic city",
        "style": "active",
        "price": "from 30$ and more"
    },
    {
        "name": "zoo",
        "style": "active",
        "price": "from 40$ and more"
    },
    {
        "name": "Attraction",
        "style": "active",
        "price": "from 50$ and more"
    },
    {
        "name": "Spectacle",
        "style": "chill",
        "price": "from 60$ and more"
    },
    {
        "name": "Cinema",
        "style": "chill",
        "price": "from 15$ and more"
    },
      {
        "name": "Cinema",
        "style": "chill",
        "price": "from 15$ and more"
    }
]

let activityBlock = document.querySelector(".activity_block")
let ActivitySetNum = 0
const maxItemActivity = 6

function displayActivities() {
    // Fade out
    activityBlock.classList.add('fade-out')

    setTimeout(() => {
        let start = ActivitySetNum * maxItemActivity
        let end = start + maxItemActivity

        let currentActivities = activity.slice(start, end)

        activityBlock.innerHTML = currentActivities.map(element => `
            <div class="activity">
                <img src="../images/tashkent.jpg">
                <div class="activity_info">
                    <span>NAME: ${element.name}</span>
                    <span>STYLE: ${element.style}</span>
                    <span>TYPE:</span>
                    <span>PRICES: ${element.price}</span>
                </div>
            </div>
        `).join('')

        activityBlock.classList.remove('fade-out')

        ActivitySetNum++

        if (start + maxItemActivity >= activity.length) {
            ActivitySetNum = 0
        }
    }, 500) // Attend la fin du fade out
}

displayActivities()
setInterval(displayActivities, 5000)