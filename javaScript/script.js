const slides = document.querySelector('.slides');
const boxes = Array.from(document.querySelectorAll('.box'));
const next = document.querySelector('.arrow.right');
const prev = document.querySelector('.arrow.left');
const buttons = document.querySelectorAll('.more');
const drop = document.getElementById('Drop');
const dropdown = drop.parentElement;
const content = dropdown.querySelector('.dropdown-content');

drop.addEventListener('click', (e) => {
  content.classList.toggle('show');
  e.stopPropagation();
});


window.addEventListener('click', (e) => {
  if (!dropdown.contains(e.target)) {
    content.classList.remove('show');
  }
});

let index = 0;

function updateCarousel() {
  const boxWidth = boxes[0].offsetWidth + 40; // width + gap
  const offset = (window.innerWidth / 2) - (boxWidth / 2) - index * boxWidth;
  slides.style.transform = `translateX(${offset}px)`;

  boxes.forEach((box, i) => {
    if (i === index) {
      box.classList.add('active');
      box.style.transform = 'scale(1.2)';
      box.style.opacity = '1';
      box.style.zIndex = '10';
    } else if (i === index - 1 || i === index + 1) {
      box.classList.remove('active');
      box.style.transform = 'scale(0.9)';
      box.style.opacity = '0.8';
      box.style.zIndex = '5';
    } else {
      box.classList.remove('active');
      box.style.transform = 'scale(0.8)';
      box.style.opacity = '0.5';
      box.style.zIndex = '1';
    }
  });
}

next.addEventListener('click', () => {
  index = (index + 1) % boxes.length;
  updateCarousel();
});

prev.addEventListener('click', () => {
  index = (index - 1 + boxes.length) % boxes.length;
  updateCarousel();
});

boxes.forEach((box, i) => {
  box.addEventListener('click', () => {
    index = i;
    updateCarousel();
  });
});

buttons.forEach((btn) => {
  btn.addEventListener('click', (e) => {
    e.stopPropagation();
    const box = btn.closest('.box');
    box.classList.toggle('show-overlay');
  });
});

window.addEventListener('resize', updateCarousel);
updateCarousel();

async function signup(username, email, password) {
  const res = await fetch("/signup", {
    method: "POST",
    headers: {"Content-Type":"application/json"},
    body: JSON.stringify({username, email, password})
  });
  return res.json();
}

// Sign-in
async function signin(username, password) {
  const res = await fetch("/signin", {
    method: "POST",
    headers: {"Content-Type":"application/json"},
    body: JSON.stringify({username, password})
  });
  return res.json();
}

async function submitQuiz(username, quizData) {
  const res = await fetch("/quiz_submit", {
    method: "POST",
    headers: {"Content-Type":"application/json"},
    body: JSON.stringify({username, quiz: quizData})
  });
  return res.json();
}

async function getFilteredActivities(quizData) {
  const res = await fetch("/filtered_activities", {
    method: "POST",
    headers: {"Content-Type":"application/json"},
    body: JSON.stringify({quiz: quizData})
  });
  return res.json();
}


const signupForm = document.querySelector("#signupForm");
if (signupForm) {
  signupForm.addEventListener("submit", async (e) => {
    e.preventDefault();
    const username = document.querySelector("#name").value;
    const email = document.querySelector("#email").value;
    const password = document.querySelector("#password") ? document.querySelector("#password").value : "1234";

    const res = await signup(username, email, password);
    if (res.status === "ok") {
      alert("Sign-up successful!");
      localStorage.setItem("username", username); // save logged-in user
      window.location.href = "/quiz";
    } else {
      alert(res.message);
    }
  });
}

const quizForm = document.querySelector("#quizForm");
if (quizForm) {
  quizForm.addEventListener("submit", async (e) => {
    e.preventDefault();
    const username = localStorage.getItem("username");
    if (!username) { alert("User not found"); return; }

    const cities = Array.from(document.querySelectorAll('input[name="cities"]:checked')).map(el => el.value);
    const categories = Array.from(document.querySelectorAll('input[name="categories"]:checked')).map(el => el.value);
    const budget = parseInt(document.querySelector("#budget").value) || 0;
    const age = parseInt(document.querySelector("#age").value) || 0;
    const nickname = document.querySelector("#nickname").value || "";

    const quizData = {cities, categories, budget, age, nickname};
    const res = await submitQuiz(username, quizData);

    if (res.status === "ok") {
      localStorage.setItem("quizData", JSON.stringify(quizData));
      window.location.href = "/results";
    } else {
      alert("Failed to submit quiz");
    }
  });
}

// Display filtered activities on results.html
const resultsContainer = document.querySelector("#resultsContainer");
if (resultsContainer) {
  const quizData = JSON.parse(localStorage.getItem("quizData") || "{}");
  getFilteredActivities(quizData).then(activities => {
    resultsContainer.innerHTML = activities.map(act => `
      <div class="activity">
        <img src="/images/${act.city.toLowerCase()}.jpg" alt="${act.name}">
        <h3>${act.name}</h3>
        <p>City: ${act.city}</p>
        <p>Categories: ${act.category.join(", ")}</p>
        <p>Price: ${act.price}</p>
        <p>Description: ${act.description}</p>
      </div>
    `).join('');
  });
}