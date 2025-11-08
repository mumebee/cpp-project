const slides = document.querySelector('.slides');
const boxes = Array.from(document.querySelectorAll('.box'));
const next = document.querySelector('.arrow.right');
const prev = document.querySelector('.arrow.left');
const buttons = document.querySelectorAll('.more');

let index = 0;

function updateCarousel() {
  const boxWidth = boxes[0].offsetWidth + 40; // ширина + gap
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
    box.classList.toggle('show-overlay'); // включаем/выключаем overlay
  });
});

window.addEventListener('resize', updateCarousel);
updateCarousel();