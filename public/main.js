let date;
let count = 0; 
let userDates = []; 
var socket = io();
let light = '0';

var today = new Date().toISOString().split('T')[0];
document.getElementById("myDate").setAttribute('min', today);

function getDate() {
  date = new Date()
  date = date[Symbol.toPrimitive]('string');
  document.querySelector('.the-paragraph').innerHTML = `${date}`;
};

function formatDate(input) {
  var parts = input.split('-');
  var date = new Date();
  date.setFullYear(parts[0]);
  date.setMonth(parts[1] - 1); // Subtract 1 from the month
  date.setDate(parts[2]);
  
  // Format the date
  var dayNames = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
  var monthNames = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
  var dayName = dayNames[date.getDay()];
  var monthName = monthNames[date.getMonth()];
  var formattedDate = `${dayName} ${monthName} ${date.getDate()} ${date.getFullYear()}`;
  
  return formattedDate;
}

setInterval(getDate, 1000); 
setInterval(checkReminders, 1000); 

socket.emit('toggleLed', '0')

document.querySelector('.clear')
  .addEventListener('click', () => {
    userDates = []; 
    document.querySelector('.registered-dates')
      .innerHTML = ''; 
    count = 0; 
  });

//check if userDates has something that starts with the date 
function disableButton() {
  const button = document.querySelector('.submit');
  button.disabled = true;
  setTimeout(function() {
    button.disabled = false;
  }, 3000);
}

document.querySelector('.submit')
  .addEventListener('click', (e) => {
    e.preventDefault(); 
    let dateValue = document.querySelector('.calendar').value
    dateValue = formatDate(dateValue) + ' ' + document.querySelector('.time-input').value; 
    for (let i = 0; i < userDates.length; i++) {
      if (userDates[i] === dateValue) {
        document.querySelector('.checking').innerHTML = 'Please do not add the same date twice.';
        return;
      }
    }
    if (dateValue.includes('undefined') || dateValue.includes('NaN')) {
      console.log(dateValue); 
      document.querySelector('.checking').innerHTML = 'Please enter proper credentials'; 
    } else {     
      userDates.push( dateValue );
      document.querySelector('.checking').innerHTML = 'Logged!'; 
      document.querySelector('.registered-dates').innerHTML += `<p class="date-${count}">${document.querySelector('.pill-name').value + ' ' + userDates[count] }</p>`; 
      count++

    }
    disableButton();
  });    

function checkReminders() {
  for (let i = 0; i < userDates.length; i++) {
    if (date.startsWith(userDates[i])) {
      document.querySelectorAll(`.date-${i}`).forEach(value => { 
        value.classList.add('completed')
      })
      userDates.splice(i, 1); 
      console.log(userDates); 
      count--;
      console.log(count, i); 
      socket.emit('toggleLed', '1');
      setTimeout(() => {
        socket.emit('toggleLed', '0')
      }, 2000)
    }
  };
}


//clear box after submit 
// document.querySelector('.calendar')
//   .addEventListener('change', () => {
//   });
// document.querySelector('.time-input')
//   .addEventListener('change', () => {
//   });