<?php

// $Id  $

// the script will use this to remove useless WAP Gateway chatter
define("REMOVE_REGEX",'/\s?UP\.Link\/[0-9\. ]+/');
/**
 * If true, matching will stop if the user agent prefix cannot
 * be matched.  The user agent prefix is the characters before
 * the first '/'.  For example, consider this user agent:
 * SonyEricssonK700i/R2AC SEMC-Browser/4.0.2 Profile/MIDP-2.0 Configuration/CLDC-1.1
 * The user agent prefix is: "SonyEricssonK700i".  If set to
 * true, and there were no devices with user agent prefixes matching
 * this one, the script would consider this to be non-matching.
 * If it was set to false, the string would continue to be matched
 * and could result in a match like "SonyEricsson", which would
 * force the script to decide which device to use - this is risky.
 * 
 * Default: true
 */
define("MATCH_TO_UA_PREFIX", true);

class wurfl {
	/**
	 * Internal tracking of the WURFL ID
	 * @var string
	 * @access private
	 */
	var $id="";

	/**
	 * If true, Openwave's GUI (mostly wml 1.3) is supported
	 * @var bool
	 * @access public
	 */
	var $GUI=false;

	/**
	 * Device brand (manufacturer)
	 * @var string
	 * @access public
	 */
	var $brand='';

	/**
	 * Device model name
	 * @var string
	 * @access public
	 */
	var $model='';

	/**
	 * If this is a WAP device, this is set to true
	 * @var boolean
	 * @access public
	 */
	var $browser_is_wap=false;

	/**
	 * associative array with all the device's capabilities.
	 * 
	 * Example:  $this->capabilities['downloadfun']['downloadfun_support'] 
	 *	true if downloadfun is supported, otherwise false
	 *
	 * @var associative array
	 * @access public
	 */
	var $capabilities=array();

	/**
	 * HTTP_ACCEPT request headers
	 * Use this to manually set the http-accept string:
	 * 
	 * Example:  $this->http_accept = "text/vnd.wap.wml";
	 * @var string
	 * @access public
	 */
	var $http_accept="";

	/**
	 * Ignore desktop browser
	 * Set this to false if you want to search the WURFL/patch
	 * for desktop web browsers as well.
	 * 
	 * @var string
	 * @access public
	 */
	var $ignoreBrowser=false;

	/**
	 * Track errors
	 * Anytime a LOG_ERR level event occurs, a description is
	 * added to the end of the array.
	 * Example:  echo count($this->$errors); // echos number of errors
	 * @var array
	 * @access public
	 */
	var $errors = array();
	
	/**
	 * WURFL ID of the ancestoral device root
	 * This is the ID of the actual device, not a firmware revision
	 *
	 * @var string
	 * @access public
	 */
	var $device_root = '';
	
	/**
	 * Device image path and filename, relative to the class file
	 *
	 * @var string
	 * @access public
	 */
	var $device_image = '';
	
	/**
	 * True if the UA was found in the cache
	 * 
	 * @var boolean
	 * @access public
	 */
	var $found_in_cache = false;
		
	function getDeviceCapabilities($_user_agent) {
		$cached = cache_get($user_agent);
		
		if(!$cached->data) {
			$_user_agent = trim(preg_replace(REMOVE_REGEX, "", $_user_agent));
			$this->user_agent = $_user_agent;
			// TODO: Spend some time on this code and make it MUCH more robust.
			// FIXME: I'm not sure that this even actually does anything :( - I think 'browser_is_wap'
			// get's overwritten anyway. Hmmmm....  Also, I changed it from LOG_WARNING to LOG_NOTICE
			if ($_check_accept == true) {
				if (!eregi('wml', $this->http_accept) && !eregi('wap', $this->http_accept) && !eregi('xhtml', $this->http_accept)) {
					//$this->_toLog('getDeviceCapabilitiesFromAgent', 'This browser does not support wml, wap, or xhtml', LOG_NOTICE);
					$this->browser_is_wap=false;
				}else{
					// We can assume this is a mobile device since it accepts wml || wap || xhtml
					$this->browser_is_wap=true;
				}
			}
			//$this->_toLog('getDeviceCapabilitiesFromAgent', 'searching for '.$_user_agent, LOG_INFO);
			if(strpos($_user_agent,'/') === false){
				// NO '/' IN USER AGENT??? This is not a WAP device (probably not a real device at all)
				// Version 1.5.2 - changed this to LOG_NOTICE since it is a normal occurence
				//$this->_toLog('getDeviceCapabilitiesFromAgent', 'Invalid user agent', LOG_NOTICE);
				$this->_setGlobalProps();
				return($this->_getGenericID($_user_agent));
			}
			
			$curr_device = $this->_UserAgentInDB($this->user_agent);
			if(is_array($curr_device)){
				// the exact user agent was in the WURFL - Great!
				$this->_GetFullCapabilities($curr_device['deviceID']);
				$this->capabilities['matched_at']=$this->user_agent;
				$this->_AddUAToCache($this->user_agent,$this->capabilities,$this->device_root);
				$this->_setGlobalProps();
				return(true);
			}

			$_ua_len = strlen($_user_agent);
			//$this->_toLog('getDeviceCapabilitiesFromAgent', 'Searching the DB ('.$this->devtable.')', LOG_INFO);
			// The user_agent should not become shorter than 4 characters
			$_min_len = 4;
			if(MATCH_TO_UA_PREFIX){
				$_min_len = strpos($_user_agent,'/');
			}
			$matched_dev = NULL;
			$niceua = rtrim(substr($_user_agent, 0, $_min_len))."%";
			$minquery = "SELECT id FROM {wurfl_devices} WHERE user_agent LIKE '%s' ORDER BY actual_device_root";
			$result = db_query($minquery, $niceua);
			$num_rows = db_result(db_query("SELECT COUNT(*) FROM {wurfl_devices} WHERE user_agent LIKE '%s' ORDER BY actual_device_root", $niceua));
			
			if($num_rows == 0){
				//DEBUG: echo "no devices match the UA down to the min chars in the DB<br />$minquery";
				// no devices match the UA down to the min chars in the DB
				// basically you're not going to get a match.
				// look for acceptable generic ID
				$this->_setGlobalProps();
				return($this->_getGenericID($_user_agent));
			}else if($num_rows == 1){
		      // Only one match... No use searching
		      // Christian Aune Thomassen
		      $matched_dev = db_fetch_object($result)->id;
		      $this->capabilities['matched_at']=substr($_user_agent, 0, $_min_len);
		      $_short_ua = $_user_agent;
		    }else{
				// $_ua_len should not be longer than the longest agent which
				// matches $niceua to reduce the amount of queries
				// by Christian Aune Thomassen
				unset($result);
				$max_user_agent_len = db_result(db_query("SELECT MAX(CHAR_LENGTH(user_agent)) AS max_user_agent FROM {wurfl_devices} WHERE user_agent LIKE '%s'", $niceua));

			
				if ($max_user_agent_len < $_ua_len) {
					// the longest user agent that we will come across is smaller than
					// this user agent - let's not waste our time here!
					$_ua_len = $max_user_agent_len;
				}
				while ( $_ua_len > $_min_len) {
					//DEBUG: echo "--trying user agent length $_ua_len<br />";
					// version 1.5.2, realized that it should be ($_ua_len - 1)
					$_short_ua = substr($_user_agent, 0, $_ua_len-1);
					// take the user agent and prep it: escapes chars and adds single quotes
					// then remove the right most quote and put %' in it's place which will
					// make it this MOT- look like this: 'MOT-%' - that will work for MySQL LIKE queries
					$niceua = rtrim($_short_ua)."%";
					// May 25 2007 - added order by and limit for accuracy and speed
					$result = db_query("SELECT id FROM {wurfl_devices} WHERE user_agent LIKE '%s' ORDER BY actual_device_root DESC LIMIT 1", $niceua);
					
					if($matched_dev = db_fetch_object($result)){
						// TODO: add $this->matched_at = $_short_ua
						$matched_dev = $matched_dev->id;
						break;
					}
					// shortening the agent by one each time
					$_ua_len--;
				}
			}
			if(!is_null($matched_dev)){
				//$device = $this->_UserAgentInDB($matched_ua);
				$this->_GetFullCapabilities($matched_dev);
				$this->capabilities['matched_at']=$_short_ua;
				$this->_AddUAToCache($this->user_agent,$this->capabilities,$this->device_root);
				//$this->_toLog('getDeviceCapabilitiesFromAgent', 'Match found in DB after '.$this->num_queries.' queries', LOG_INFO);
				$this->_setGlobalProps();
				return(true);
			}else{
				// no match was found
				$this->capabilities['matched_at']=$_short_ua;
				$this->_AddUAToCache($this->user_agent,$this->capabilities,$this->device_root);
				//$this->_toLog('getDeviceCapabilitiesFromAgent', 'No match was found in DB >>THIS SHOULDN\'T EVER HAPPEN!<<', LOG_WARNING);
				$this->_setGlobalProps();
				return(false);
			}			
		}
	}
	
	/**
	 * This function checks the user agent for signs that it's a WAP device
	 * Returns true if a generic ID is found, otherwise false.
	 * This is a last resort function that is only called if the device in question
	 * does not exist in the WURFL and the class is forced to find another way to
	 * identify the device.
	 *
	 * @param string User agent
	 * @access private
	 * @return boolean success
	 */
	function _getGenericID($_user_agent){
		if(RETURN_GENERIC === false){
			$this->capabilities['matched_at']='';
			return false;
		}
		//$this->_toLog('getGenericID', "I couldn't find the device in my list, the headers are my last chance", LOG_NOTICE);
		$matched_dev = '';
		$my_is_wireless = false;
		if ( strstr($_user_agent, 'UP.Browser/') && strstr($_user_agent, '(GUI)') ) {
			$matched_dev = 'upgui_generic';
			$my_is_wireless = true;
		} else if ( strstr($_user_agent, 'UP.Browser/') ) {
			$matched_dev = 'uptext_generic';
			$my_is_wireless = true;
		} else if ( eregi('wml', $this->http_accept) || eregi('wap', $this->http_accept) ) {
			$matched_dev = 'generic';
			$my_is_wireless = true;
		} else {
			//$this->_toLog('getGenericID', 'This should not be a WAP device, quitting', LOG_NOTICE);
			$matched_dev = 'generic';
			$my_is_wireless = false;
		}
		$this->_GetFullCapabilities($matched_dev);
		$this->capabilities['matched_at']='';
		$this->capabilities['product_info']['is_wireless_device']=$my_is_wireless;
		$this->_setGlobalProps();
		// we should return false if this device is generic and not wireless
		// this "unmatched device" is not cached because there is no logical way to tell
		// that it is unmatched when we read it from the cache.
		if($my_is_wireless === false) return false;
		//$this->_AddUAToCache($this->user_agent,$this->capabilities,$this->device_root);
		return true;
	}
	
	function _setGlobalProps(){
		$this->GUI = false;
		if(!isset($this->capabilities['product_info']['is_wireless_device'])){
			$this->capabilities['product_info'] = array();
			// set this to false by default
			$this->capabilities['product_info']['is_wireless_device'] = false;
			$this->capabilities['product_info']['brand_name'] = '';
			$this->capabilities['product_info']['model_name'] = '';
		}
		unset($this->wurfl_agent);
		unset($this->browser_is_wap);
		unset($this->brand);
		unset($this->model);
		unset($this->id);
		$this->wurfl_agent = $this->capabilities['user_agent'];
		$this->browser_is_wap = $this->capabilities['product_info']['is_wireless_device'];
		$this->brand = $this->capabilities['product_info']['brand_name'];
		$this->model = $this->capabilities['product_info']['model_name'];
		$this->id = $this->capabilities['id'];
	}	
	
	
	function _GetFullCapabilities($_id) {
		if(count($this->errors) != 0) return(false);
		//$this->_toLog('_GetFullCapabilities', "searching for $_id", LOG_INFO);
		$_curr_device = $this->_getDeviceCapabilitiesFromId($_id);
		// array of full records
		$_capabilities[] = $_curr_device;
		// keep the while loop from running away on an error
		$iteration_limit = 20;
		$i = 0;
		while ( $_curr_device['fall_back'] != 'generic' && $_curr_device['fall_back'] != 'root' && $i <= $iteration_limit) {
			//$this->_toLog('_GetFullCapabilities', 'parent device:'.$_curr_device['fall_back'].' now going to read its capabilities', LOG_INFO);
			$_curr_device = $this->_getDeviceCapabilitiesFromId($_curr_device['fall_back']);
			array_unshift($_capabilities,$_curr_device);
			$i++;
		}
		if($i >= $iteration_limit){
			// the while loop ran away
			//$this->_toLog('_GetFullCapabilities', 'Killing runaway while loop - $_id='.$_id, LOG_ERR);
			return(false);
		}
		//$this->_toLog('_GetFullCapabilities', 'getting baseline capabilities from generic device', LOG_INFO);
		$generic = $this->_getDeviceCapabilitiesFromId('generic');
		$_final = $generic;
		// the generic devices are already at the top of the array because I used array_unshift()
		foreach($_capabilities as $curr_device){
			//TODO: Why don't I just array_merge the whole record???? Good question!
			foreach($curr_device as $key => $val) {
				if ( is_array($val) ) {
					$_final[$key] = array_merge($_final[$key], $val);
				} else {
					$_final[$key] = $val;
				}
			}
		}
		$this->capabilities = $_final;
		//$this->brand = $this->capabilities['product_info']['brand_name'];
		//$this->model = $this->capabilities['product_info']['model_name'];
		//$this->id = $this->capabilities['id'];
		return(true);
	}

	/**
	 * Given a device id reads its capabilities
	 *
	 * @param string device's wurfl_id
	 * @access private
	 * @return mixed boolean false if not identified or array capabilities
	 *
	 */
	function _getDeviceCapabilitiesFromId($_id) {
		if(count($this->errors) != 0) return(false);
		//$this->_toLog('_getDeviceCapabilitiesFromId', "reading id:$_id", LOG_INFO);
		if ( $_id == 'upgui_generic' ) {
			$this->GUI = true;
		}
		$result = db_query("SELECT * FROM {wurfl_devices} WHERE id = '%s'", $_id);
		$num_rows = db_result(db_query("SELECT COUNT(*) FROM {wurfl_devices} WHERE id = '%s'", $_id));
		
		if($num_rows > 0){
			$device = db_fetch_array($result);
			//print_r($device);
			if($this->device_root == '' && $device['actual_device_root'] == 1){
				//$this->_toLog("_getDeviceCapabilitiesFromId","device root detected: ".$device['deviceID'], LOG_INFO);
				$this->device_root = $device['deviceID'];
				//$image = IMAGE_DIR.$device['deviceID'].".gif";
				// PHP evaluates from left to right, so "file_exists" will not get
				// called if IMAGE_CHECKING is false
				/*if(IMAGE_CHECKING && file_exists($image)){
					$this->device_image = $image;
					$this->_toLog("_getDeviceCapabilitiesFromId","device image found: $image",LOG_INFO);
				}*/
			}
			$cap = unserialize($device['capabilities']);
			//echo "<pre>".print_r($cap,true)."</pre>";
			return($cap);
		}else{
			// device is not in the WURFL
			// deal with it appropriately
			//$this->_toLog('_getDeviceCapabilitiesFromId', "the id $_id is not present in wurfl >>HOW DID THIS HAPPEN???<<", LOG_ERR);
			//die("the id $_id is not present in wurfl_agents");
			// I should never get here!!
			return(false);
		}
	}	
	/**
	 * Checks to see if a given user agent is in the WURFL database
	 * 
	 * @param string user agent
	 * @return mixed false if not in DB, else full device record array
	 * @access private
	 */
	function _UserAgentInDB($ua){
		//TODO: using LIKE will do the search case-insensitive, but = is much faster
		$query = db_query("SELECT * FROM {wurfl_devices} WHERE LOWER(user_agent) = '%s'", drupal_strtolower($ua));
		$device = db_fetch_object($query);
		$return = ($device)? $device : false;
		return $return;
	}
	/**
	 * Checks to see if a given user agent is in the cache
	 * 
	 * @param string user agent
	 * @return mixed false if not in cache, else full device cache data array
	 * @access private
	 */
	function _UserAgentInCache($ua){
		return cache_get($ua);
	}
	/**
	 * Adds a user agent and its associated data to the cache
	 * 
	 * @param string user agent
	 * @param array capabilities
	 * @param string device root
	 * @return mixed false if not in cache, else full device cache data array
	 * @access private
	 */
	function _AddUAToCache($ua,$cap,$devroot){
		if(!WURFL_CACHE_ENABLE)return(true);
		$ua = trim(preg_replace(REMOVE_REGEX, "", $ua));
		// don't cache UAs that are too short to be real
		if(strlen($ua) < 3)return(true);
		$data = serialize(array("capabilities"=>$cap,"device_root"=>$devroot));
		return cache_set($ua, $data);
	}	
	
	/**
	 * This allows PHP < 4.3.0 to use Tera-WURFL
	 * 
	 * @param mixed Input variable to be prepared
	 * @access private
	 */
	function _compatibleEscape($value){
		if (version_compare(phpversion(), "4.3.0", ">=")) { 
		  // you're on 4.3.0 or later 
		  return(mysql_real_escape_string($value));
		} else { 
		  // you're not 
		  return(mysql_escape_string($value));
		}
	}	
}