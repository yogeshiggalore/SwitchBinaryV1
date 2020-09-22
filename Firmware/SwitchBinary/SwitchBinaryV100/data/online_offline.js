let onUpdate    = document.getElementById('onUpdate');
let text        = document.getElementById('text');
let icon        = document.getElementById('ion-icon');

window.addEventListener('online', function(){
    if(icon.classList.contains('ion-md-close-circle')){
        icon.classList.remove('ion-md-power');
        icon.classList.add('ion-md-power');
    }
    text.innerHTML = "You're online";
    onUpdate.style.color = '#27ae60';
});

window.addEventListener('offline', function(){
    if(icon.classList.contains('ion-md-power')){
        icon.classList.remove('ion-md-power');
        icon.classList.add('ion-md-close-circle');
    }
    text.innerHTML = "You're offline";
    onUpdate.style.color = '#c0392b';
});