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
	 * Point component for A-Frame.
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
	  multiple: true,

	  /**
	   * Called once when component is attached. Generally for initial setup.
	   */
	  init: function () {
	    // Create geometry.
	    this.geometry = new THREE.Geometry();
	    for (i=0;i<50000;i++) {
	      this.geometry.vertices.push(new THREE.Vector3(0, 0, 0));
	    }
	    // Create material.
	    this.material = new THREE.PointsMaterial({
	      color: this.data.color,
	      size: this.data.size,
	      sizeAttenuation: this.data.perspective,
	      blending: this.data.blending,
	      transparent: this.data.transparent
	    });
	    // Create mesh.
	    this.points = new THREE.Points(this.geometry, this.material);
	    // Set mesh on entity.
	    this.el.setObject3D('mesh', this.points);
	  },

	  setPoints: function (points) {
			const pallet = [60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 0];			
			var colors = [];

	    this.geometry = new THREE.Geometry();
	    var vertices = this.geometry.vertices;
	    points.forEach(function (point) {
				vertices.push(new THREE.Vector3(point[0], point[1], point[2]));
				var c = "hsl(" + pallet[point[3]] + ", 100%, 50%)";
				colors.push(new THREE.Color(c));
			});
			this.geometry.colors = colors;

			this.material = new THREE.PointsMaterial({
				size: this.data.size, 
				sizeAttenuation: this.data.perspective,
				opacity: this.data.opacity, 
				transparent: this.data.transparent, 
				vertexColors: THREE.VertexColors
			});
			
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

	AFRAME.registerPrimitive('a-point', {
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