<?php
namespace Drupal\registration_views;

/**
 * A Views' field handler for editing the registration state.
 */
class RegistrationViewsHandlerStateEdit extends views_handler_field {

  function construct() {
    parent::construct();
    $this->additional_fields['registration_id'] = 'registration_id';
    $this->additional_fields['state'] = 'state';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  /**
   * Render the field contents.
   */
  function render($values) {
    // Render a Views form item placeholder.
    return '<!--form-item-' . $this->options['id'] . '--' . $this->view->row_index . '-->';
  }

  /**
   * Add to and alter the form.
   */
  function views_form(&$form, &$form_state) {
    // Create a container for our replacements
    $form[$this->options['id']] = array(
      '#type' => 'container',
      '#tree' => TRUE,
    );
    // Iterate over the result and add our replacement fields to the form.
    foreach ($this->view->result as $row_index => $row) {
      // Add a text field to the form.  This array convention
      // corresponds to the placeholder HTML comment syntax.
      $form[$this->options['id']][$row_index] = array(
        '#type' => 'select',
        '#default_value' => $row->{$this->aliases['state']},
        '#options' => registration_get_states_options(),
        '#required' => TRUE,
      );
    }
  }

  /**
   * Form submit method.
   */
  function views_form_submit($form, &$form_state) {
    // Determine which nodes we need to update.
    $updates = array();
    foreach ($this->view->result as $row_index => $row) {
      $value = $form_state['values'][$this->options['id']][$row_index];
      if ($row->{$this->aliases['state']} != $value) {
        $updates[$row->{$this->aliases['registration_id']}] = $value;
      }
    }

    $registrations = registration_load_multiple(array_keys($updates));
    foreach ($registrations as $registration_id => $registration) {
      $registration->state = $updates[$registration_id];
      registration_save($registration);
    }

    drupal_set_message(t('Updated @num registration states.', array('@num' => count($updates))));
  }

}
