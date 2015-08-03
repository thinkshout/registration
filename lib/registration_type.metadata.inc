<?php

/**
 * @file
 * @see hook_entity_property_info()
 */
class RegistrationTypeMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['registrant_entity_type'] = array(
      'label' => t('Registrant Entity Type'),
      'type' => 'text',
      'description' => t("The entity type this registration registers."),
      'required' => TRUE,
      'schema field' => 'registrant_entity_type',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    );
    $properties['registrant_bundle'] = array(
      'label' => t('Registrant Bundle'),
      'type' => 'text',
      'description' => t("The bundle this registration registers."),
      'required' => TRUE,
      'schema field' => 'registrant_bundle',
      'setter callback' => 'entity_property_verbatim_set',
    );
    $properties['registrant_email_property'] = array(
      'label' => t('Registrant Email Property'),
      'type' => 'text',
      'description' => t("The unique ID of the registration."),
      'required' => TRUE,
      'schema field' => 'registrant_email_property',
      'setter callback' => 'entity_property_verbatim_set',
    );

    return $info;
  }

}
