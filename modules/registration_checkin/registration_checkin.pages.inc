<?php
/**
 * @file
 * Page callbacks for Entity Registration Checkin.
 */

/**
 * Page callback: Show a list of active registrations that can be checked in.
 */
function registration_checkin_list_page($entity_type, $entity) {
  // Get the host entity info.
  list($entity_id) = entity_extract_ids($entity_type, $entity);
  $label = $entity->label();

  // Build the registrant listing (or lack of one).
  $pager_limit = 20;
  $registrations
    = registration_checkin_get_all_registrations($entity_id, $entity_type, $pager_limit);

  if ($registrations) {
    $table = array(
      'header' => registration_checkin_get_registration_table_headers(),
      'rows' => registration_checkin_get_registration_table_rows($registrations),
    );
    // @FIXME
// theme() has been renamed to _theme() and should NEVER be called directly.
// Calling _theme() directly can alter the expected output and potentially
// introduce security issues (see https://www.drupal.org/node/2195739). You
// should use renderable arrays instead.
// 
// 
// @see https://www.drupal.org/node/2195739
// $registrant_markup = theme('table', $table) . theme('pager');

  }
  else {
    $registrant_markup = t('There are no registrants for %name', array('%name' => $label));
  }

  // Get the search form.
  $search_form = \Drupal::formBuilder()->getForm('registration_checkin_search_form', $entity_type, $entity_id);

  // Get the registrants table.
  $registrants_build
    = registration_checkin_prepare_registrants_checkin_build($registrant_markup);

  // Create a button to checkin an unregistered attendee
  // by registering them as immediately attending.
  $new_checkin_button = array(
    '#theme' => 'link',
    '#text' => t('New Registrant Check-in'),
    '#path' => $entity_type . '/' . $entity_id . '/register/attended',
    '#options' => array(
      'attributes' => array('class' => 'button'),
      'html' => FALSE,
    ),
  );

  // Build the page as a render array.
  $page = array(
    'content' => array(
      'new_button' => $new_checkin_button,
      'search_form' => $search_form,
      'registrant_table' => $registrants_build,
    ),
  );
  return $page;
}
