/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};

/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {

/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId])
/******/ 			return installedModules[moduleId].exports;

/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			exports: {},
/******/ 			id: moduleId,
/******/ 			loaded: false
/******/ 		};

/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);

/******/ 		// Flag the module as loaded
/******/ 		module.loaded = true;

/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}


/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;

/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;

/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";

/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(0);
/******/ })
/************************************************************************/
/******/ ([
/* 0 */
/***/ (function(module, exports) {

  /* global AFRAME THREE */

  if (typeof AFRAME === 'undefined') {
    throw new Error('Component attempted to register before AFRAME was available.');
  }

  /**
   * Heatmap component for A-Frame.
   */
  AFRAME.registerComponent('point', {
    schema: {
      size: {
        type: 'number',
        default: 1
      },
      perspective: {
        type: 'boolean',
        default: true
      },
      opacity: {
        type: 'number',
        default: 1.0
      },
      transparent: {
        type: 'boolean',
        default: false
      }
    },

    /**
     * Set if component needs multiple instancing.
     */
    multiple: false,

    /**
     * Called once when component is attached. Generally for initial setup.
     */
    init: function () {
      this.material = new THREE.PointsMaterial({
        size: this.data.size, 
        sizeAttenuation: this.data.perspective,
        opacity: this.data.opacity, 
        transparent: this.data.transparent, 
        vertexColors: THREE.VertexColors
      });
    },

    setPoints: function (points) {
      const pallet = [
        "hsl(60, 100%, 50%)",
        "hsl(55, 100%, 50%)",
        "hsl(50, 100%, 50%)",
        "hsl(45, 100%, 50%)",
        "hsl(40, 100%, 50%)",
        "hsl(35, 100%, 50%)",
        "hsl(30, 100%, 50%)",
        "hsl(25, 100%, 50%)",
        "hsl(20, 100%, 50%)",
        "hsl(15, 100%, 50%)",
        "hsl(0, 100%, 50%)"
      ];
      var colors = [];

      this.geometry = new THREE.Geometry();
      var vertices = this.geometry.vertices;
      points.forEach(function (point) {
        vertices.push(new THREE.Vector3(point[0], point[1], point[2]));
        colors.push(new THREE.Color(pallet[point[3]]));
      });
      this.geometry.colors = colors;

      // Create mesh.
      this.points = new THREE.Points(this.geometry, this.material);
      
      // Set mesh on entity.
      this.el.setObject3D('mesh', this.points);
    },

    /**
     * Called when a component is removed (e.g., via removeAttribute).
     * Generally undoes all modifications to the entity.
     */
    remove: function () {
      this.el.removeObject3D('mesh');
    }

  });

  AFRAME.registerPrimitive('a-heatmap', {
    defaultComponents: {
      point: {}
    },
    mappings: {
      size: 'point.size',
      perspective: 'point.perspective',
      opacity: 'point.opacity',
      transparent: 'point.transparent',
    }
  });


/***/ })
/******/ ]);