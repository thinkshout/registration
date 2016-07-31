<?php

/**
 * @file
 * Contains registration_state.page.inc.
 *
 * Page callback for Registration state entities.
 */

use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Prepares variables for Registration state templates.
 *
 * Default template: registration_state.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_registration_state(array &$variables) {
  // Fetch RegistrationState Entity Object.
  $registration_state = $variables['elements']['#registration_state'];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}
