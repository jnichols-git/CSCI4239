// Identity matrix
function mat4_identity() {
  // Generate identity matrix
  mat = [
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  ];
  return mat;
}

// Translation matrix
function mat4_translate(x,y,z) {
  // Generate translation matrix
  var mat = [
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    x, y, z, 1
  ];
  return mat;
}

// Scale matrix
function mat4_scale(x,y,z) {
  // Generate scale matrix
  var mat = [
    x, 0, 0, 0,
    0, y, 0, 0,
    0, 0, z, 0,
    0, 0, 0, 1
  ];
  return mat;
}

// Rotation matrix
function mat4_rotate(th, x,y,z) {
  // Normalize axis of rotation
  var len = Math.sqrt(x*x+y*y+z*z);
  if(len==0) return;
  x /= len;
  y /= len;
  z /= len;
  // Calculate sine and cosine of rotation angle
  var s = Math.sin(th*Math.PI/180);
  var c = Math.cos(th*Math.PI/180);
  // Generate rotation matrix
  var mat =
  [
      (1-c)*x*x+c  , (1-c)*x*y+z*s, (1-c)*z*x-y*s, 0,
      (1-c)*x*y-z*s, (1-c)*y*y+c  , (1-c)*y*z+x*s, 0,
      (1-c)*z*x+y*s, (1-c)*y*z-x*s, (1-c)*z*z+c  , 0,
      0,             0,             0,             1
  ];
  return mat;
}

// Perspective matrix
function mat4_perspective(fovy, asp, zrat) {
  // Cotangent of fov angle
  var s = Math.sin(fovy/2*Math.PI/180);
  var c = Math.cos(fovy/2*Math.PI/180);
  if (s==0) return;
  var cot = c/s;
  var zFar  = zrat*32.0;
  var zNear = zrat/32.0;
  var Zs = -(zFar+zNear)/(zFar-zNear);
  var Z0 = -2*zNear*zFar/(zFar-zNear);
  // Generate perspective projection matrix
  var mat = [
    cot/asp, 0  , 0 , 0,
    0      , cot, 0 , 0,
    0      , 0  , Zs, -1,
    0      , 0  , Z0, 0
  ];
  return mat;
}

// Calculate normal from modelview.
// Adapted from graphics gems II.
function mat3_normal(ModelView) {
  // Calculate the determinant of upper left 3x3 submatrix
  var det = ModelView[0]*ModelView[5]*ModelView[10]
           +ModelView[1]*ModelView[6]*ModelView[8]
           +ModelView[2]*ModelView[4]*ModelView[9]
           -ModelView[2]*ModelView[5]*ModelView[8]
           -ModelView[1]*ModelView[4]*ModelView[10]
           -ModelView[0]*ModelView[6]*ModelView[9];
  //  Compute inverse using Cramer's rule
  var inv = [1,0,0 , 0,1,0 , 0,0,1];
  if (det*det>1e-25)
  {
     inv[0] =  (ModelView[5]*ModelView[10]-ModelView[6]*ModelView[9])/det;
     inv[1] = -(ModelView[4]*ModelView[10]-ModelView[6]*ModelView[8])/det;
     inv[2] =  (ModelView[4]*ModelView[ 9]-ModelView[5]*ModelView[8])/det;
     inv[3] = -(ModelView[1]*ModelView[10]-ModelView[2]*ModelView[9])/det;
     inv[4] =  (ModelView[0]*ModelView[10]-ModelView[2]*ModelView[8])/det;
     inv[5] = -(ModelView[0]*ModelView[ 9]-ModelView[1]*ModelView[8])/det;
     inv[6] =  (ModelView[1]*ModelView[ 6]-ModelView[2]*ModelView[5])/det;
     inv[7] = -(ModelView[0]*ModelView[ 6]-ModelView[2]*ModelView[4])/det;
     inv[8] =  (ModelView[0]*ModelView[ 5]-ModelView[1]*ModelView[4])/det;
  }
  return inv;
}

// Right multiply a into b
function mat4_multiply(a, b) {
  var mat = [];
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++) {
      mat[4*i+j] = a[j]*b[4*i] + a[4+j]*b[4*i+1] + a[8+j]*b[4*i+2] + a[12+j]*b[4*i+3];
    }
  }
  return mat;
}

var key_left = key_right = false;
function keyDown(event) {
  if(event.keyCode == 39) {
    key_right = true;
  }
  else
  if(event.keyCode == 37) {
    key_left = true;
  }
}
function keyUp(event) {
  if(event.keyCode == 39) {
    key_right = false;
  }
  else
  if(event.keyCode == 37) {
    key_left = false;
  }
}