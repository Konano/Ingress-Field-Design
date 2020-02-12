// ==UserScript==
// @id             iitc-plugin-portalsinpolygons@hayeswise
// @name           IITC plugin: Portals-in-Polygons
// @category       Layer
// @version        1.2017.02.24
// @namespace      https://github.com/hayeswise/ingress-intel-total-conversion
// @description    Display a list of portals in, on on the perimeter of, polygons and circles, and on lines.  Use the layer group check boxes to filter the portals.
// @updateURL      https://github.com/hayeswise/iitc-shadowops/raw/master/dist/plugins/wise-portalsinpolygons.meta.js
// @downloadURL    https://github.com/hayeswise/iitc-shadowops/raw/master/dist/plugins/wise-portalsinpolygons.user.js
// @include        https://intel.ingress.com/*
// @include        http://intel.ingress.com/*
// @match          https://intel.ingress.com/*
// @match          http://intel.ingress.com/*
// @require        https://rawgit.com/hayeswise/Leaflet.PointInPolygon/v1.0.0/wise-leaflet-pip.js
// @author         Hayeswise
// @grant          none
// ==/UserScript==
// MIT License, Copyright (c) 2017 Brian Hayes ("Hayeswise")
// For more information, visit https://github.com/hayeswise/iitc-wise-portalsinpolygon
/* a
 * Thanks to:
 * IITC - Ingress Intel Total Conversion - https://iitc.me/ and https://github.com/iitc-project/ingress-intel-total-conversion
 *   License: https://github.com/iitc-project/ingress-intel-total-conversion/blob/master/LICENSE
 * Leaflet.Geodesic by Kevin Brasier (a.k.a. Fragger)
 *   License: https://github.com/Fragger/Leaflet.Geodesic/blob/master/LICENSE
 *   Leaflet.Geodesc has been extended by the IITC project.  See the IITC distribution of the L.Geodesc.js in GitHub.
 * Dan Sunday's Winding Number and isLeft C++ implementation - http://geomalgorithms.com/.
 *   Copyright and License: http://geomalgorithms.com/a03-_inclusion.html
 */
 /**
 * Greasemonkey/Tampermonkey information about the plugin.
 * @typedef ScriptInfo
 * @type {Object}
 * @property {String} version This is set to GM_info.script.version.
 * @property {String} name This is set to GM_info.script.name.
 * @property {String} description This is set to GM_info.script.description.
 */
/**
 * Plugin information which includes the Greasemonkey/Tampermonkey information about the plugin.
 * @typedef PluginInfo
 * @type {Object}
 * @property {ScriptInfo} script Greasemonkey/Tampermonkey information about the plugin.
 */
/**
  * The IITC map object (a Leaflet map).
  * @external "window.map"
  * @see {@link https://iitc.me/ Ingress Intel Total Conversion}
  */
/**
  * The IITC portals object (used as a map) that contains a list of the cached
  * portal information for the portals in the current and surrounding view.
  * @type {Object<string, Object>} Assoiciative array of portal information keyed by the portal's guid.
  * @external "window.portals"
  * @see {@link https://iitc.me/ Ingress Intel Total Conversion}
  */
/**
  * The map data render class which handles rendering into Leaflet the JSON data from the servers.  Needed to access
  * `window.Render.prototype.bringPortalsToFront`.
  * @external "window.Render"
  * @see {@link https://iitc.me/ Ingress Intel Total Conversion}
  */
/**
 * The "show list of portals" plugin object, properties, and methods.
 * @external "window.plugin.portalslist"
 * @see {@link http://leafletjs.com/ "show list of portals"} plugin source code for further information.
 */

/**
 * Required Plugins helper.
 * @module {function} "window.helper.requiredPlugins"
 */
 ;(function () {
  "use strict";
  /**
   * Information about a plugin.  The `pluginKey` is the property name of the
   * plugin in the `window.plugin` associative array.  The `name` value is used
   * in messaging about the plugins (e.g., if it is missing).
   * @typedef PluginMetaData
   * @type {Object}
   * @property {String} pluginKey The property name of the plugin in
   *  `window.plugin`.
   * @property {String} name A title or short name of the plugin.
   * @example
   * {
   *   pluginKey: "drawTools",
   *   name: "draw tools"
   * }
   */
  // Aggregate helpers in the window.helper object
  if (typeof window.helper !== "object") {
    window.helper = {};
  }
  window.helper.requiredPlugins = {};
  /**
   * Required Plugins namespace.
   * @alias "window.helper.requiredPlugins"
   * @variation 2
   */
  var self = window.helper.requiredPlugins;
  self.spacename = "helper.requiredPlugins";
  self.version = "0.1.0";

  /**
   * Returns true if all the prerequisite plugins are installed.
   * @param {PluginMetaData[]} prerequisites An array of
   * `RequiredPluginMetaData`.
   * @returns {boolean} Returns `true` if all the prerequisite plugins are
   *  installed; otherwise, returns `false`.
   * @example
   * window.plugin.myPlugin.requiredPlugins = [{
   *   pluginKey: window.plugin.drawTools,
   *   name: "draw tools"
   * }, {
   *   pluginKey: window.plugin.myotherplugin,
   *   name: "My Other Plugin"
   * }]
   * ...
   * if (window.helper.requiredPlugins.areMissing(window.plugin.myPlugin.requiredPlugins)) {
   *    return;
   * }
   */
  self.areMissing = function (prerequisites) {
    var areMissing;
    areMissing = prerequisites.some(function (metadata) {
      return (typeof window.plugin[metadata.pluginKey] === "undefined");
    });
    return areMissing;
  };

  /**
   * Checks if the prerequisite/required plugins are installed.
   * @param {PluginMetaData[]} requiredPlugins An array of plugin meta-data on
   * the required plugins.
   * @returns {PluginMetaData[]}
   * @example
   * window.plugin.myPlugin.requiredPlugins = [{
   *   pluginKey: window.plugin.drawTools,
   *   name: "draw tools"
   * }, {
   *   pluginKey: window.plugin.myotherplugin,
   *   name: "My Other Plugin"
   * }]
   * ...
   * var missing = window.helper.requiredPlugins.missingPluginNames(window.plugin.myPlugin.requiredPlugins);
   * if (missing.length > 0) {
   *   msg = 'IITC plugin "' + pluginName + '" requires IITC plugin' + ((missing.length === 1) ? ' ' : 's ') +
   *     ((missing.length === 1) ? missing[0] : (missing.slice(0,-1).join(", ") + " and " + missing[missing.length - 1])) + '.';
   *   console.warn(msg);
   *   alert(msg);
   * }
   */
  self.missingPluginNames = function (requiredPlugins) {
    var missing = [];
    requiredPlugins.forEach(function (metadata) {
      if (metadata.pluginKey === undefined) {
        missing.push('"' + metadata.name + '"');
      }
    });
    return missing;
  };

  /**
   * Checks if the pre-requisite plugins are installed.  If one or more requisites are not installed, an alert is
   * displayed.
   * @param {RequiredPluginMetaData[]} requiredPlugins An array of plugin meta-data on the required plugins.
   * @param {string} pluginName The name of the plugin requiring the required plugins.  Recommend using
   *    `plugin_info.script.name`.
   * @returns {boolean}
   * @example
   * window.plugin.myPlugin.requiredPlugins = [{
   *   pluginKey: window.plugin.drawTools,
   *   name: "draw tools"
   * }, {
   *   pluginKey: window.plugin.myotherplugin,
   *   name: "My Other Plugin"
   * }]
   * ...
   * if (!window.helper.requiredPlugins.alertIfNotInstalled(window.plugin.myPlugin.requiredPlugins, plugin_info.script.name) {
   *    return;
   * }
   */
  self.alertIfNotInstalled = function (requiredPlugins, pluginName) {
    var missing = [],
      msg;
    missing = self.missingPluginNames(requiredPlugins);
    if (missing.length > 0) {
      msg = 'IITC plugin "' + pluginName + '" requires IITC plugin' + ((missing.length === 1) ? ' ' : 's ') +
        ((missing.length === 1) ? missing[0] : (missing.slice(0, -1).join(", ") + " and " + missing[missing.length - 1])) + '.';
      window.console.warn(msg);
      alert(msg);
    }
    return (missing.length === 0);
  };
}());

/**
 * Toolbox Control Section helper.
 * @module {function} "window.helper.ToolboxControlSection"
 */
;(function () {
  "use strict";
  // Aggregate helpers in the window.helper object
  if (typeof window.helper !== "object") {
    window.helper = {};
  }

  /**
   * ToolboxControlSection Class.  Provides a standardized way of adding toolbox controls and grouping controls in
   * the same "family".
   */
  /**
   * Creates a new ToolboxControlSection.
   *
   * @class
   * @param {String|Element|Text|Array|jQuery} content A object suitable for passing to `jQuery.append()`: a
   *  DOM element, text node, array of elements and text nodes, HTML string, or jQuery object to insert at the end of
   *  each element in the set of matched elements.
   * @param {String} controlSectionClass The class name for a section of controls, typically in a `div` tag.
   * @param {String} [controlClass] An optional class name of a simple control or collection of controls.
   * @property {String} defaultStyle Global CSS for the toolbox control section.  Set
   *  using `setStyle()`.
   * @property {String} style Global CSS for the toolbox control section.  Set
   *  using `setStyle()`.
   */
  window.helper.ToolboxControlSection = function (content, controlSectionClass, controlClass) {
    this.controlSectionClass = controlSectionClass;
    this.controlClass = controlClass;
    this.merged = false;
    this.jQueryObj = jQuery('<div>').append(content).addClass(controlSectionClass);
  };
  // Properties
  var self = window.helper.ToolboxControlSection;
  self.version = "0.1.0";
  self.defaultStyle = "div.wise-toolbox-control-section {color:#00C5FF;text-align:center;width:fit-content;border-top: 1px solid #20A8B1;border-bottom: 1px solid #20A8B1;}";
  self.style = undefined;
  /**
   * See jQuery `.attr()` function.
   *
   * @returns {String}
   * @todo Consider removing this.
   */
  self.prototype.attr = function (attributeNameOrAttributes, valueOrFunction) {
    if (typeof valueOrFunction === 'undefined') {
      return this.jQueryObj.attr(attributeNameOrAttributes);
    } else {
      return this.jQueryObj.attr(attributeNameOrAttributes, valueOrFunction);
    }
  };

  /**
   * Appends toolbox controls with the same toolbox control section class and toolbox control class.
   * <p>
   * Merge
   * ```
   * <div class="myControlFamily">
   *    ...this control...
   * </div>
   * ```
   * with
   * ```
   * <div class="myControlFamily">
   *    ...other control...
   * </div>
   * ```
   * to get
   * ```
   * <div class="myControlFamily">
   *    ...this control...
   *    ...other control...
   * </div>
   * ```
   */
  self.prototype.mergeWithFamily = function () {
    var controlFamily,
      that;
    if (!this.merged) {
      that = this;
      controlFamily = jQuery('.' + this.controlSectionClass);
      if (controlFamily.length > 0) {
        controlFamily.each(function () {
          var jQobj = jQuery(this);
          jQobj.css("border-style", "none");
          that.jQueryObj.append(jQobj.removeClass(that.controlSectionClass).addClass(that.controlSectionClass + "-moved")); // remove oringal section so any subsequent merges have a single control section to deal with
        });
        this.merged = true;
      }
      if (typeof this.controlClass !== 'undefined') {
        controlFamily = jQuery(':not(.' + this.controlSectionClass + ') .' + this.controlClass);
        if (controlFamily.length > 0) {
          controlFamily.each(function () {
            that.jQueryObj.append(jQuery(this));
          });
          this.merged = true;
        }
      }
    }
    return this.jQueryObj;
  };

  /**
   * Sets the documents's styling.  Will not add the style if previously used.
   * @param {String} [styling] CSS styles.
   */
  self.prototype.setStyle = function (styling) {
    if (typeof styling === "undefined") {
      styling = self.defaultStyle;
    }
    if (typeof self.style === 'undefined' || (self.style !== styling)) {
      self.style = styling;
      jQuery("<style>")
        .prop("type", "text/css")
        .html(styling)
        .appendTo("head");
    }
  };

  /**
   * Override valueOf so that we get the desired behavior of getting the jQuery object when we access an object
   * directly.
   * @returns {Object} jQuery object.
   * @example
   * $("#toolbox").append(new ToolboxControlSection(html, "myfamily-control-section", "myfamily-control").mergeWithFamily();
   */
  self.prototype.valueOf = function () {
    return this.jQueryObj;
  };
}());


/**
 * Portals-in-Polygon IITC plugin.  The plugin and its members can be accessed via
 * `window.plugin.portalsinpolygons`.
 * @module {function} "window.plugin.portalsinpolygons"
 */
/**
 * Closure function for Portals-in-Polygon.
 * <p>
 * Standard IITC wrapper pattern used to create the plugin's closure when
 * "installed" using `document.createElement("script".appendChild(document.createTextNode('('+ wrapper +')('+JSON.stringify(info)+');'));`
 * @param {PluginInfo} plugin_info Plugin information object provided the standard IITC PLUGINEND code.
 */

;function wrapper(plugin_info) {
"use strict";// ensure plugin framework is there, even if iitc is not yet loaded
if(typeof window.plugin !== 'function') window.plugin = function() {};

//PLUGIN AUTHORS: writing a plugin outside of the IITC build environment? if so, delete these lines!!
//(leaving them in place might break the 'About IITC' page or break update checks)
plugin_info.buildName = 'wise';
plugin_info.dateTimeVersion = '20170226.80142';
plugin_info.pluginId = 'wise-portalsinpolygons';
//END PLUGIN AUTHORS NOTE


// PLUGIN START ///////////////////////////////////////////////////////////////
    window.plugin.portalsinpolygons = function () {};
  /**
   * Portals-in-Polygon namespace.  `self` is set to `window.plugin.portalsinpolygons`.
   * @alias "window.plugin.portalsinpolygons"
     * @variation 2
   */
    var self = window.plugin.portalsinpolygons;
    self.spacename = "portalsinpolygons";
    // Configuration
    self.title = "Portals-in-Polygon";
    self.version = "";
    /**
   * An array of objects describing the required plugins.
     * @type {RequiredPluginMetaData[]} Array of required plugin meta-data.
   */
    self.requiredPlugins = [{
        pluginKey: "drawTools",
        name: "draw tools"
    }, {
        pluginKey: "portalslist",
        name: "show list of portals"
    }];

    /**
     * A assoicative array of layer chooser names.
   * Used when calling `window.isLayerGroupDisplayed(<String> name)`.
     * @example
   * window.isLayerGroupDisplayed(self.layerChooserName[portal.options.data.level])
     * @type {Object.<string, string>}
   */
    self.layerChooserName = {
        0: "Unclaimed/Placeholder Portals",
        1: "Level 1 Portals",
        2: "Level 2 Portals",
        3: "Level 3 Portals",
        4: "Level 4 Portals",
        5: "Level 5 Portals",
        6: "Level 6 Portals",
        7: "Level 7 Portals",
        8: "Level 8 Portals",
        Resistance: "Resistance",
        Enlightened: "Enlightened",
        Neutral: "Unclaimed/Placeholder Portals"
    };

    /**
     * Returns a list of portals contained in the geodesic polygon. An optional
     * associative array of portals (like window.portals) can be provided to
     * restict the base list (e.g., to find if bookmarked portals are in the
     * polygoon).
     * @param {Object<string, Object>} [portals] Optional. An associative array
     *  of IITC portal objects keyed by the portal guid.
     *  Defaults to `window.portals`.
     * @member external:L.Polyline.portalsIn
     * @returns {Object<string, Object>} An associative array of portal
     *  objects in the polygon.
     */
    L.Polyline.prototype.portalsIn = function (portals) {
        var fname = "L.Polynline.prototype.portalsIn";
        var containedPortals,
            keys,
            rectangularBounds,
            point,
            portal,
            wn;
        console.log (fname + ": Start");
        portals =(typeof portals === "undefined") ? window.portals : portals;
        keys = Object.keys(portals);
        rectangularBounds = this.getBounds();
        containedPortals = new Map();
        console.log ("---");
        console.log(["Index","Title","GUID","Lng(X)","Lat(Y)","WindingNumber"].join(","));
        for (var i = 0; i < keys.length; i++) {
            portal = portals[keys[i]];
            point = L.latLng(portal.options.data.latE6 / 1E6, portal.options.data.lngE6 / 1E6);
            if (this.contains(point)) {
                containedPortals[portal.options.guid] = portal;
                console.log ([i, '"' + portal.options.data.title + '"', portal.options.guid, portal.options.data.lngE6 / 1E6, portal.options.data.latE6 / 1E6, wn].join(","));
            }
        }
        console.log ("---");
        console.log (fname + ": End");
        return containedPortals;
    };

    /**
   * Bring portals to the front of the draw layers so that you can click on
   * them after drawing a circle or polygon over the portals.
   * <br>
   * Thanks to Zaso's "Bring Portals To Front" at
   * <a href="http://www.giacintogarcea.com/ingress/iitc/bring-portals-to-front-by-zaso.meta.js"> Zaso Items</a>.
   */
    self.bringPortalsToFront = function(){
        window.Render.prototype.bringPortalsToFront(); // See IITC code
    };

    /**
   * A getPortalsCallback function returns returns an associative array of IITC portals (typically a subset
   * of `window.portals`).
   *
   * @callback getPortalsCallback
   * @returns {Object<string,Object>} An associative array of IITC portals.
   * @see {@link displayPortals}
   * @see {@link displayContainedPortals}
     */

    /**
   * Displays portals.  The portals are filtered based on selections in the layer chooser.
   * <br>
   * This function is generalized version of the `window.plugin.portalslist.displayPL` function.
   * @param {getPortalsCallback} [getPortalsFn] Optional. An callback function that returns an associative array of IITC
   *  portals. If the function is not provided or set to undefined, the portals in the current map bounds will be
   *  used.
     * @param {String} [title="Portal List"] Optional. A title for the portal list dialog.  The default is
   * "Portal list".
   */
    self.displayPortals = function (getPortalsFn, title) {
        var fname = self.spacename + ".displayPortals";
        var formattedPortals,
            list,
            msg,
            portals,
            type;
        // Check parameters.
        type = typeof getPortalsFn;
        if (type !== 'function') {
            if (type === 'undefined') {
                getPortalsFn = self.getPortalsInMapBounds;
            } else {
                msg = "Unexpected parameter type, '" + type + "', for function " + fname + ", parameter getPortalsFn.";
                throw new TypeError (msg, plugin_info.name);
            }
        }
        title = (typeof title === 'undefined' ? "Portal list" : title);

        if (!self.mapZoomHasPortals()) {
            console.warn("Map is zoomed too far out to get sufficient portal data (e.g., the portal name).");
            list = $('<table class="noPortals"><tr><td>Please zoom to get additional portal data like the portal title.</td></tr></table>');
        } else {
            // plugins (e.g. bookmarks) can insert fields before the standard ones - so we need to search for the 'level' column
            window.plugin.portalslist.sortBy = window.plugin.portalslist.fields.map(function (f) {
                return f.title;
            }).indexOf('Level');
            window.plugin.portalslist.sortOrder = -1;
            window.plugin.portalslist.enlP = 0;
            window.plugin.portalslist.resP = 0;
            window.plugin.portalslist.neuP = 0;
            window.plugin.portalslist.filter = 0;

            // Get portals and format them for display.
            portals = getPortalsFn.call(this);
            formattedPortals = self.formattedPortalList(portals);
            if (formattedPortals.length > 0) {
                list = window.plugin.portalslist.portalTable(window.plugin.portalslist.sortBy, window.plugin.portalslist.sortOrder, window.plugin.portalslist.filter);
            } else {
                list = $('<table class="noPortals"><tr><td>Nothing to show!</td></tr></table>');
            }
        }
        // Display table of portals.
        if (window.useAndroidPanes()) {
            $('<div id="portalslist" class="mobile">').append(list).appendTo(document.body);
        } else {
            dialog({
                html: $('<div id="portalslist">').append(list),
                dialogClass: 'ui-dialog-portalslist',
                title: title + ': ' + window.plugin.portalslist.listPortals.length + ' ' + (window.plugin.portalslist.listPortals.length == 1 ? 'portal' : 'portals'),
                id: 'portalsinpolygons-list',
                width: 700
                }
            );
        }
    };

    /**
   * Displays the portals contain in, and on the perimeter, of drawn polygons
   * and on any lines.
   */
    self.displayContainedPortals = function () {
        self.displayPortals(self.getContainedPortals, "Portals-in-Polygons");
    };
    var list;
    self.outputAsJson = function() {
      //self.displayPortals(self.getContainedPortals, "outPut-as-Json");
        window.plugin.portalslist.sortBy = window.plugin.portalslist.fields.map(function (f) {
              return f.title;
          }).indexOf('Level');
        window.plugin.portalslist.sortOrder = -1;
        window.plugin.portalslist.enlP = 0;
        window.plugin.portalslist.resP = 0;
        window.plugin.portalslist.neuP = 0;
        window.plugin.portalslist.filter = 0;

        // Get portals and format them for display.
        portals = self.getContainedPortals.call(this);
        var formattedPortals = self.formattedPortalList(portals);
        var node = {};
        node.maps = {};
        node.maps.idOthers = {};
        node.maps.idOthers.label = "Others";
        node.maps.idOthers.state = window.plugin.portalslist.listPortals.length;
        node.maps.idOthers.bkmrk = {};

        node.portals = {};
        node.portals.idOthers = {};
        node.maps.idOthers.label = "Others";
        node.portals.idOthers = {};
        node.portals.idOthers.bkmrk = {};
        for (var idx = 0; idx < window.plugin.portalslist.listPortals.length; ++idx) {
          var tee = {
            guid: window.plugin.portalslist.listPortals[idx].sortValues[0],
            latlng: String(window.plugin.portalslist.listPortals[idx].portal._latlng.lat) + "," + String(window.plugin.portalslist.listPortals[idx].portal._latlng.lng),
            label: window.plugin.portalslist.listPortals[idx].sortValues[1],
          }
          node.portals.idOthers.bkmrk[String(tee.latlng)] = tee;
        }
        // console.log('============');
        // console.log(JSON.stringify(node));
        var ls = "<textarea readonly style='width:700'>" + JSON.stringify(node) + '</textarea>';
        //console.log(window.plugin.portalslist.listPortals);
        // console.log('============');
//        if (formattedPortals.length > 0) {
//            list = window.plugin.portalslist.portalTable(window.plugin.portalslist.sortBy, //window.plugin.portalslist.sortOrder, window.plugin.portalslist.filter);
//        } else {
//            list = $('<table class="noPortals"><tr><td>Nothing to show!</td></tr></table>');
//        }
//        console.log(list);
//        console.log('============');
//        console.log(typeof list);
//        //alert(list.stringify());
//        // Display table of portals.
/*        if (window.useAndroidPanes()) {
            $('<div id="portalslist" class="mobile">').append(list).appendTo(document.body);
        } else {
*/            dialog({
                html: $('<div id="portalslist">').append(ls),
                dialogClass: 'ui-dialog-portalslist',
                title: JSON + ': ' + window.plugin.portalslist.listPortals.length + ' ' + (window.plugin.portalslist.listPortals.length == 1 ? 'portal' : 'portals'),
                id: 'portalsinpolygons-list',
                width: 700
                }
            );
//        }
    }

    /**
   * Gets and formats the portal information that will be used in the portal list display.
   * <br>
   * This function is based on a modified version of the
   * `window.plugin.portalslist.getPortals` function.
   * @param {Object} portals An associative array of IITC portals.
   * @returns {Array<{portal:{Object}, values:{Array}, sortValues:{Array}>} Returns an array of
   *  formatted portals.
   */
    self.formattedPortalList = function (portals) {
        //filter : 0 = All, 1 = Neutral, 2 = Res, 3 = Enl, -x = all but x
        var fname = self.spacename + "formattedPortalList";
        var guids, // {String[]}
        msg, // {String}
            portalList = [];
        guids = Object.keys(portals);
        console.log ("---");
        console.log(["Index","Title","GUID","Lng(X)","Lat(Y)"].join(","));

        guids.forEach(function (guid, i, array) {
            var portal = portals[guid];
            console.log ([i, '"' + portal.options.data.title + '"', portal.options.guid, portal.options.data.lngE6 / 1E6, portal.options.data.latE6 / 1E6].join(","));
            switch (portal.options.team) {
                case window.TEAM_RES:
                    window.plugin.portalslist.resP++;
                    break;
                case window.TEAM_ENL:
                    window.plugin.portalslist.enlP++;
                    break;
                default:
                    window.plugin.portalslist.neuP++;
            }

            // cache values and DOM nodes
            var obj = {
                portal: portal,
                values: [],
                sortValues: []
            };

            var row = document.createElement('tr');
            row.className = window.TEAM_TO_CSS[portal.options.team];
            obj.row = row;

            var cell = row.insertCell(-1);
            cell.className = 'alignR';

            window.plugin.portalslist.fields.forEach(function (field, i) {
                cell = row.insertCell(-1);

                var value = field.value(portal);
                if (typeof value === 'undefined') {
                    value = "[unknown]";
                }
                obj.values.push(value);

                obj.sortValues.push(field.sortValue && !!value ? field.sortValue(value, portal) : value);

                if (field.format) {
                    field.format(cell, portal, value);
                } else {
                    cell.textContent = value;
                }
            });

            portalList.push(obj);
        });
        console.log ("---");
        window.plugin.portalslist.listPortals = portalList;
        return portalList;
    };

    /**
   * Returns an array of IITC portals contained in the polygons and circles
   * drawn on the map.<br>
   * Checks for layers of type L.Polygon, which includes L.GeodesicPolygon
   * and L.GeodesicCircle, and L.Polyline, which in L.GeodesicPolyline.
   * @param {(keepPortalCallback|true|false)} [keepPortalFn = window.plugin.portalsinpolygons.isPortalDisplayed] If a
   * callback function is
   *  provided, it will be called and passed the IITC portal object. If keepPortalFn is not a function and is set to
   *  something falsy, the portals will not be filtered.  If keepPortalCallback is not provided, explicitly
   *  undefined, or something truthy, then the default filtering will be
   *  performed which is to filter portals based on the layer group selections of "Unclaimed Portals",
   *  "Level 1 Portals" to "Level 8 Portals", "Enlightened" and "Resistance".
   * @returns {Object} A collection of IITC portals.
   */
    self.getContainedPortals = function(keepPortalFn) {
        var fname = self.spacename + ".getContainedPortals";
        console.log (fname + ": Start");
        var enclosures,
            enclosureData,
            data = [],
            layers, // Leaflet Layer[]
            type,
            portals;
        // Check parameter
        type = typeof keepPortalFn;
        if (type !== 'function') {
            if (type === 'undefined') {
                keepPortalFn = self.isPortalDisplayed;
            } else if (!keepPortalFn) {
                keepPortalFn = function (portal) {return true;};
            } else {
                keepPortalFn = self.isPortalDisplayed;
            }
        }
        // Loop through all map elements looking for polygons and circles (and in the future polylines).
        layers = window.plugin.drawTools.drawnItems.getLayers();
        console.log(fname + ": layers.length=" + layers.length);
        enclosures = layers.filter(function(layer, i, array) {
            return (layer instanceof L.Polygon || layer instanceof L.Polyline);
        });

        portals = enclosures.reduce(function(collectedPortals, layer, currentIndex, array){
            var morePortals;
            var desc = [currentIndex, self.getLayerClassName(layer)];
            // start debug
            var latLngs = layer.getLatLngs();
            latLngs.forEach(function(latLng, i, array) {
                data.push([].concat(desc).concat([latLng.lng, latLng.lat]));
            });
            // end debug
            morePortals = (typeof layer.portalsIn === 'function') ? layer.portalsIn() : {};
            //return collectedPortals.concat(morePortals);
            return jQuery.extend(collectedPortals, morePortals);
        }, Object.create(null));
        // start debug
        console.log(fname + ": Number of enclosures: " + enclosures.length);
        console.log ("---");
        console.log(["LayerNumber","LayerClassName","Lng(X)","Lat(Y)"].join(","));
        data.forEach(function(elem) {
            console.log (elem.join(","));
        });
        console.log ("---");
        // end debug
        // Filter out unwanted portals
        portals = Object.keys(portals).reduce(function(collectedPortals, guid, currentIndex, array) {
            var portal = portals[guid];
            if (keepPortalFn(portal)) {
                collectedPortals[portal.options.guid] = portal;
            }
            return collectedPortals;
        }, Object.create(null));
        console.log (fname + ": End");
        return portals;
    };

    /**
   * Returns a string representation of the layer class (e.g., "L.GeodesicPolygon" and "L.Marker").
   * @param {L.Layer} layer An object whose class extends L.Layer.
   * @returns {String} A string representation of the layer class.
   */
    self.getLayerClassName = function getLayerClassName (layer) {
        if (layer instanceof L.GeodesicCircle) {
            return "L.GeodesicCircle";
        } else if (layer instanceof L.GeodesicPolygon) {
            return "L.GeodesicPolygon";
        } else if (layer instanceof L.GeodesicPolyline) {
            return "L.GeodesicPolyline";
        } else if (layer instanceof L.Circle) {
            return "L.Circle";
        } else if (layer instanceof L.Marker) {
            return "L.Marker";
        } else if (layer instanceof L.Polygon) {
            return "L.Polygon";
        } else if (layer instanceof L.Polyline) {
            return "L.Polyline";
        } else {
            return "New or Unknown Layer Type";
        }
    };

    /**
   * Returns a set of guids belonging to the portals filtered by the layer group selections of
   * "Unclaimed Portals", "Level 1 Portals" to "Level 8 Portals", "Enlightened" and "Resistance".
   * @param {Object} portals An associative array of IITC portal objects.
   * @returns {string[]} An array of portal guids.
   */
    self.getPortalGuidsFilteredByLayerGroup = function (portals) {
        var guids;
        guids = Object.keys(portals);
        guids = guids.filter(function (guid, i, array) {
            var keep;
            var portal = portals[guid];
            keep = (window.isLayerGroupDisplayed(self.layerChooserName[portal.options.data.level]) &&
                    ((portal.options.data.team === "R" && window.isLayerGroupDisplayed(self.layerChooserName.Resistance)) ||
                     (portal.options.data.team === "E" && window.isLayerGroupDisplayed(self.layerChooserName.Enlightened)) ||
                     (portal.options.data.team === "N" && window.isLayerGroupDisplayed(self.layerChooserName.Neutral))));
            return keep;
        });
        return guids;
    };

    /**
   * A keepPortalCallback function returns true if the the provided portal passes the test implemented by the
   * callback function.  The callback is used to determine if the portal should be displayed in the list of portals.
   *
   * @callback keepPortalCallback
   * @param {Object} portal An IITC portal object.
   * @returns {boolean} True if the portal should be kept.  False if the portal should be ignored.
   * @see {@link getPortalsInMapBounds}
     */

    /**
   * Returns the portals within the displayed map boundaries.
   * @param {(keepPortalCallback|true|false)} [keepPortalFn = self.isPortalDisplayed] If a callback function is
   *  provided, it will be called and passed the IITC portal object. If keepPortalFn is not a function and is set to
   *  something falsy, the portals will not be filtered.  If keepPortalCallback is not provided, explicitly
   *  undefined, or something truthy, then the default filtering will be
   *  performed which is to filter portals based on the layer group selections of "Unclaimed Portals",
   *  "Level 1 Portals" to "Level 8 Portals", "Enlightened" and "Resistance".
   * @returns {Object} An associative array of IITC portal objects (a subset of `window.portals`).
   */
    self.getPortalsInMapBounds = function (keepPortalFn) {
        var displayBounds,
            type,
            boundedPortals;
        // Check parameter
        type = typeof keepPortalFn;
        if (type !== 'function') {
            if (type === 'undefined') {
                keepPortalFn = self.isPortalDisplayed;
            } else if (!keepPortalFn) {
                keepPortalFn = function (portal) {return true;};
            } else {
                keepPortalFn = self.isPortalDisplayed;
            }
        }
        displayBounds = window.map.getBounds(); // the bounds could contain larger than life lat and lngs if zoomed out far.
    displayBounds.getSouthWest().wrap();
        displayBounds.getNorthEast().wrap();

        boundedPortals = Object.keys(window.portals).reduce(function (collectedPortals, guid, currentIndex, array) {
            var portal;
            portal = window.portals[guid];
            //      var exp = {latLng: portal.getLatLng(), contains: displayBounds.contains(portal.getLatLng()), keep:keepPortalFn(portal)};
            //      console.log("exp="+JSON.stringify(exp));
            if (displayBounds.contains(portal.getLatLng()) && keepPortalFn(portal)) {
                collectedPortals[guid] = portal;
            }
            return collectedPortals;
        }, Object.create(null));
        return boundedPortals;
    };

    /**
   * Returns the DOM elements containing the plugin controls to be appended to the IITC toolbox.
   * <br>
   * Controls from other plugins with class "wise-toolbox-control" or "wise-toolbox-control-section" will be grouped
   * into one subsection (same div tag).
   * @returns {Object} DOM elements.
   */
    self.getToolboxControls = function () {
    var controlsHtml,
            pluginControl,
            portalsToFrontControl,
            displayPortalsControl,
            listPortalsInPolygonControl,outputAsJson;

        portalsToFrontControl = '<span style="white-space:nowrap"><a id="portalsinpolygons-portalsToFront" onclick="window.plugin.portalsinpolygons.bringPortalsToFront();false;" title="Bring portals to the front draw layer so that you can click on them after drawing a circle or polygon over them.">Portals To Front</a></span>';
        listPortalsInPolygonControl = '<span style="white-space:nowrap"><a id="portalsinpolygons-portalsInPolygons" onclick="window.plugin.portalsinpolygons.displayContainedPortals();false;" title="Display a list of portals in polygons, circles, and on lines.  Use the layer group check boxes to filter the portals.">Portals in Polygons</a></span>';
        displayPortalsControl = '<span style="white-space:nowrap"><a id="portalsinpolygons-portalsOnMap" onclick="window.plugin.portalsinpolygons.displayPortals();false;" title="Display a list of portals.">Portals on Map</a></span>';
        outputAsJson = '<span style="white-space:nowrap"><a id="portalsinpolygons-outputAsJson" onclick="window.plugin.portalsinpolygons.outputAsJson();false;" title="Output Portals details in Json format">Output as Json</a></span>';
        controlsHtml = listPortalsInPolygonControl + '&nbsp;&#9679; ' + displayPortalsControl + '&nbsp;&#9679; ' + portalsToFrontControl + '&nbsp;&#9679; ' + outputAsJson;

    pluginControl = new window.helper.ToolboxControlSection(controlsHtml, "wise-toolbox-control-section", "wise-toolbox-control");
    pluginControl.attr("id", self.spacename + ".controls");
        pluginControl.setStyle();
    pluginControl = pluginControl.mergeWithFamily();
    return pluginControl;
    };

    /**
   * Returns the portal if it is displayed based on the the layer group selections of "Unclaimed Portals",
   * "Level 1 Portals" to "Level 8 Portals", "Enlightened" and "Resistance".  Returns null if it is not
   * displayed.
   * @param {Object} portal An IITC portal object.
   * @returns {(Object|null)} The IITC portal object or null.
   */
    self.isPortalDisplayed = function (portal) {
        var keep;
        keep = (window.isLayerGroupDisplayed(self.layerChooserName[portal.options.level]) &&
                ((portal.options.data.team === "R" && window.isLayerGroupDisplayed(self.layerChooserName.Resistance)) ||
                 (portal.options.data.team === "E" && window.isLayerGroupDisplayed(self.layerChooserName.Enlightened)) ||
                 (portal.options.data.team === "N" && window.isLayerGroupDisplayed(self.layerChooserName.Neutral))));
        return keep;
    };

    /**
   * Checks if there is sufficient portal data for the current map zoom.  When the zoom is set very far,
     * `window.portals` will only contain placeholder data and may not contain the portal title and other
   * information.
   * @todo it might be easier to check if one of the portals has the data your are looking for (e.g., check if portal.options.data.title exists).
   * @returns {boolean} True if there is sufficient portal data; otherwise, returns false.
   */
    self.mapZoomHasPortals = function() {
        var zoom = map.getZoom();
        zoom = getDataZoomForMapZoom(zoom);
        var tileParams = getMapZoomTileParameters(zoom);
        return tileParams.hasPortals;
    };

    /**
     * Setup function called by IITC.
     */
    self.setup = function init() {
        var fname = self.spacename + ".setup";
        var controls;
        self.version = (!!plugin_info ? plugin_info.script.version : "unknown");
    console.log (fname + ": Start, version " + self.version);
        if (!window.helper.requiredPlugins.alertIfNotInstalled(self.requiredPlugins, plugin_info.script.name)) {
            return;
        }
    // Standard sytling for "wise" family of toolbox controls
    $("<style>")
      .prop("type", "text/css")
      .html("div.wise-toolbox-control-section {color:#00C5FF;text-align:center;width:fit-content;border-top: 1px solid #20A8B1;border-bottom: 1px solid #20A8B1;}")
        .appendTo("head");
        // Add controls to IITC right hand side toolbox.
        controls = self.getToolboxControls();
        $("#toolbox").append(controls);

        // Delete setup function so that it is not run again.
        console.log (fname + ": End");
        delete self.setup;
    };

    /*
     * Set the required setup function that is called or handled by PLUGINEND
     * code provided IITC build script.  The function will be called if IITC is
     * already loaded and, if not, saved for later execution.
     */
    var setup = self.setup;
// PLUGIN END /////////////////////////////////////////////////////////////////

setup.info = plugin_info; //add the script info data to the function as a property
if(!window.bootPlugins) window.bootPlugins = [];
window.bootPlugins.push(setup);
// if IITC has already booted, immediately run the 'setup' function
if(window.iitcLoaded && typeof setup === 'function') setup();
} // wrapper end
// inject code into site context
var script = document.createElement('script');
var info = {};
if (typeof GM_info !== 'undefined' && GM_info && GM_info.script) info.script = {version: GM_info.script.version, name: GM_info.script.name, description: GM_info.script.description };
script.appendChild(document.createTextNode('('+ wrapper +')('+JSON.stringify(info)+');'));
(document.body || document.head || document.documentElement).appendChild(script);


