<?php

/**
 * @file
 * Contains registration.page.inc.
 *
 * Page callback for Registration entities.
 */

use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Prepares variables for Registration templates.
 *
 * Default template: registration.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_registration(array &$variables) {
  // Fetch Registration Entity Object.
  $registration = $variables['elements']['#registration'];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}
