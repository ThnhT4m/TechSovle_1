// Import the functions you need from the SDKs you need
import { initializeApp } from "https://www.gstatic.com/firebasejs/11.4.0/firebase-app.js";
import { getAuth, signInWithEmailAndPassword} from "https://www.gstatic.com/firebasejs/11.4.0/firebase-auth.js";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyC6B_cfJ5IDNfq2zDWJEiBW1MeriWjyF4w",
  authDomain: "salinguard.firebaseapp.com",
  projectId: "salinguard",
  storageBucket: "salinguard.firebasestorage.app",
  messagingSenderId: "375251537347",
  appId: "1:375251537347:web:be8140bb54d81f9d070176",
  measurementId: "G-GWK8H3REQP"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const auth = getAuth(app);

//submit button
const submit = document.getElementById('submit');
const loginBox = document.querySelector("main");
submit.addEventListener('click', function (event) {
    event.preventDefault()

    //inputs
    const email = document.getElementById('email').value;
    const password = document.getElementById('password').value;
    signInWithEmailAndPassword(auth, email, password)
    .then((userCredential) => {
        // Signed in 
        const user = userCredential.user;
        window.location.href = "../pages/grand.html";
        // ...
    })
    .catch((error) => {
        const errorMessage = error.message;
        loginBox.classList.add("shake");
        setTimeout(() => {
            loginBox.classList.remove("shake");
        }, 300);
        alert(errorMessage)
        // ..
    });
})