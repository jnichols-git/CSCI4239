function test() {
  alert(document.getElementById('tags').value);
}

var key = 'a1b89d0a730503de506899fe2249b9da';
var secret = 'a3ea989843970999';
var page = 1;

var display_callback;
var loadtex_callback;
var image_urls;
var aspects;

function makeNewApiCall() {
  page = 1;
  makeApiCall();
}

function makeApiCall() {
  //retrieve API arguments
  var tags = document.getElementById('tags').value;
  var count = 16;
  //build request
  var req = `https://www.flickr.com/services/rest/?method=flickr.photos.search&api_key=${key}&tags=${tags}&per_page=${count}&page=${page}&extras=url_sq&format=json&nojsoncallback=1`;
  //make initial request
  var images = new Array(16);
  image_urls  = new Array(16);
  aspects = new Array(16);
  $.ajax({url:req, dataType:"json"}).then(function(data) {
    images = data.photos.photo;
    for(var n=0; n<16; n++) {
      image_urls[n] = images[n].url_sq;
      aspects[n]    = 1.0;
    }
    if(loadtex_callback) loadtex_callback();
    // Iterate through IDs and grab medium-size images and aspect ratios
    for(var n=0; n<16; n++) {
      setImageWithAspect(images[n].id, n);
    }
    if(loadtex_callback) loadtex_callback();
  });
  page += 1;
  //use returned photos to populate gallery
}

function setImageWithAspect(id, index) {
  var req = `https://www.flickr.com/services/rest/?method=flickr.photos.getSizes&api_key=${key}&photo_id=${id}`;
  $.ajax({url:req, dataType:"xml"}).then(function(data) {
    var med_dat = data.activeElement.childNodes[1].childNodes[13];
    //asp = parseFloat(med_dat.getAttribute("width"))/parseFloat(med_dat.getAttribute("height"));
    image_urls[index]  = med_dat.getAttribute("source");
    aspects[index] = parseFloat(med_dat.getAttribute("width"))/parseFloat(med_dat.getAttribute("height"))
    //console.log(asp);
  });
}

// Load a texture from a URL
// https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/Tutorial/Using_textures_in_WebGL
function loadTexture(gl, url) {
  const texture = gl.createTexture();
  gl.bindTexture(gl.TEXTURE_2D, texture);

  // Because images have to be downloaded over the internet
  // they might take a moment until they are ready.
  // Until then put a single pixel in the texture so we can
  // use it immediately. When the image has finished downloading
  // we'll update the texture with the contents of the image.
  const level = 0;
  const internalFormat = gl.RGBA;
  const width = 1;
  const height = 1;
  const border = 0;
  const srcFormat = gl.RGBA;
  const srcType = gl.UNSIGNED_BYTE;
  const pixel = new Uint8Array([0, 0, 255, 255]);  // opaque blue
  gl.texImage2D(gl.TEXTURE_2D, level, internalFormat,
                width, height, border, srcFormat, srcType,
                pixel);

  const image = new Image();
  image.onload = function() {
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(gl.TEXTURE_2D, level, internalFormat,
                  srcFormat, srcType, image);

    // WebGL1 has different requirements for power of 2 images
    // vs non power of 2 images so check if the image is a
    // power of 2 in both dimensions.
    if (isPowerOf2(image.width) && isPowerOf2(image.height)) {
       // Yes, it's a power of 2. Generate mips.
       gl.generateMipmap(gl.TEXTURE_2D);
    } else {
       // No, it's not a power of 2. Turn off mips and set
       // wrapping to clamp to edge
       gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
       gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
       gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
       gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    }
    //display_callback();
  };
  image.crossOrigin = "anonymous";
  image.src = url;

  return texture;
}

function isPowerOf2(value) {
  return (value & (value - 1)) == 0;
}