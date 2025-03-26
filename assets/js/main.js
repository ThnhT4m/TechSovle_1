window.onload = function () {
  Particles.init({
    selector: "canvas.background"
  });
};
const particles = Particles.init({
  selector: "canvas.background",
  color: ["#03dac6", "#ff0266", "#000000"],
  connectParticles: true,
  maxParticles: 200, 
  responsive: [
    {
      breakpoint: 768,
      options: {
        color: ["#faebd7", "#03dac6", "#ff0266"],
        maxParticles: 100,
        connectParticles: false
      }
    }
  ]
});
