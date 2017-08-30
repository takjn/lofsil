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
	    color: {
	      type: 'color',
	      default: '#888'
	    },
	    size: {
	      type: 'number',
	      default: 1
	    },
	    perspective: {
	      type: 'boolean',
	      default: false
	    },
	    transparent: {
	      type: 'boolean',
	      default: false
	    },
	    blending: {
	      type: 'number',
	      default: THREE.NoBlending,
	      oneOf: [THREE.NoBlending,THREE.NormalBlending,THREE.AdditiveBlending,THREE.SubtractiveBlending,THREE.MultiplyBlending]
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
	    this.duration=0;
	    this.index = 0;
	  },

	  setPoint: function (point) {
	    var target = this.geometry.vertices[this.index];
	    if (target != undefined) {
	      target.x = point[0];
	      target.y = point[1];
	      target.z = point[2];
	      this.geometry.verticesNeedUpdate = true;
	    }

	    if (this.index < 50000) {
	      this.index++;
	    }
	    else {
	      this.index = 0;      
	    }
		},
		
	  setPoints: function (points) {
			const pallet = [80, 70, 60, 50, 40, 30, 20, 15, 10, 5, 0];			
			var colors = [];

	    this.geometry = new THREE.Geometry();
	    var vertices = this.geometry.vertices;
	    points.forEach(function (point) {
				vertices.push(new THREE.Vector3(point[0], point[1], point[2]));
				var c = "hsl(" + pallet[point[3]] + ", 100%, 50%)";
				colors.push(new THREE.Color(c));
			});

			this.geometry.colors = colors;
			this.material = new THREE.PointsMaterial({ size:0.02,sizeAttenuation:true,vertexColors: THREE.VertexColors});
			
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
	    color: 'point.color',
	    size: 'point.size',
	    perspective: 'point.perspective',
	    transparent: 'point.transparent',
	    blending: 'point.blending'
	  }
	});


/***/ })
/******/ ]);