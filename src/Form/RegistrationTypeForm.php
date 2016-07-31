<?php

namespace Drupal\registration\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class RegistrationTypeForm.
 *
 * @package Drupal\registration\Form
 */
class RegistrationTypeForm extends EntityForm {

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    $registration_type = $this->entity;
    $form['label'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $registration_type->label(),
      '#description' => $this->t("Label for the Registration type."),
      '#required' => TRUE,
    ];

    $form['id'] = [
      '#type' => 'machine_name',
      '#default_value' => $registration_type->id(),
      '#machine_name' => [
        'exists' => '\Drupal\registration\Entity\RegistrationType::load',
      ],
      '#disabled' => !$registration_type->isNew(),
    ];

    // Registrant Entity form fields:
    $form['registrant_entity'] = array(
      '#title' => t('Registrant entity'),
      '#type' => 'fieldset',
      '#attributes' => array(
        'id' => array('registration-registrant-entity'),
      ),
    );

    $form_entity_type = & $form_state->getValue('registrant_entity_type');

    // Prep the entity type list before creating the form item:
    $entity_types = array('' => t('-- Select --'));
    $all_entities = \Drupal::entityTypeManager()->getDefinitions();
    foreach ($all_entities as $entity_type => $entity) {

      // Ignore registration entity types:
      if (strpos($entity_type, 'registration') === FALSE) {
        $entity_types[$entity_type] = $entity->getLabel();
      }
    }
    asort($entity_types);

    if (isset($form_entity_type)) {
      if (!$entity_types[$form_entity_type]) {
        $form_entity_type = NULL;
      }
    }
    elseif (isset($entity_types[$registration_type->registrant_entity_type])) {
      $form_entity_type = $registration_type->registrant_entity_type;
    }

    $form['registrant_entity']['registrant_entity_type'] = array(
      '#title' => t('Entity type'),
      '#type' => 'select',
      '#options' => $entity_types,
      '#default_value' => $form_entity_type,
      '#required' => TRUE,
      '#description' => t('Select a registrant entity type (default: User).'),
      '#ajax' => array(
        'callback' => 'registration_mapping_form_callback',
        'wrapper' => 'registration-registrant-entity',
      ),
    );

    if ($form_entity_type) {
      $form_bundle = & $form_state->getValue('registrant_bundle');

      // Prep the bundle list before creating the form item:
      $bundles = array('' => t('-- Select --'));
      $form_entity = \Drupal::entityTypeManager()->getDefinitions($form_entity_type);

      foreach ($form_entity['bundles'] as $key => $bundle) {
        $bundles[$key] = $bundle->getLabel();
      }
      asort($bundles);

      if (isset($form_bundle)) {
        if (!$bundles[$form_bundle]) {
          $form_bundle = NULL;
        }
      }
      elseif (isset($bundles[$registration_type->registrant_bundle])) {
        $form_bundle = $registration_type->registrant_bundle;
      }

      $form['registrant_entity']['registrant_bundle'] = array(
        '#title' => t('Entity bundle'),
        '#type' => 'select',
        '#required' => TRUE,
        '#description' => t('Select a registrant entity bundle with a Drupal user id and email address.'),
        '#options' => $bundles,
        '#default_value' => $form_bundle,
        '#ajax' => array(
          'callback' => 'registration_mapping_form_callback',
          'wrapper' => 'registration-registrant-entity',
        ),
      );

      if ($form_bundle) {
        $form_email_property = & $form_state->getValue('registrant_email_property');

        // Prep the field & properties list before creating the form item:
        $fields = registration_email_fieldmap_options($form_entity_type, $form_bundle);

        // Flatten fields array by one level.
        $flattened_fields = array();
        foreach ($fields as $key => $value) {
          if (is_array($value)) {
            foreach ($value as $sub_key => $sub_value) {
              $flattened_fields[$sub_key] = $sub_value;
            }
          }
          else {
            $flattened_fields[$key] = $value;
          }
        }

        if (isset($form_email_property)) {
          if (!$flattened_fields[$form_email_property]) {
            $form_email_property = NULL;
          }
        }
        elseif (isset($flattened_fields[$registration_type->registrant_email_property])) {
          $form_email_property = $registration_type->registrant_email_property;
        }

        $form['registrant_entity']['registrant_email_property'] = array(
          '#title' => t('Email property'),
          '#type' => 'select',
          '#required' => TRUE,
          '#description' => t('Select the field which contains the email address.'),
          '#options' => $fields,
          '#default_value' => $form_email_property,
          '#maxlength' => 127,
        );
      }
    }

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $registration_type = $this->entity;
    $status = $registration_type->save();

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created the %label Registration type.', [
          '%label' => $registration_type->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Registration type.', [
          '%label' => $registration_type->label(),
        ]));
    }
    $form_state->setRedirectUrl($registration_type->urlInfo('collection'));
  }

}
