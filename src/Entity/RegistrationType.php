<?php

namespace Drupal\registration\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;

/**
 * Defines the Registration type entity.
 *
 * @ConfigEntityType(
 *   id = "registration_type",
 *   label = @Translation("Registration type"),
 *   handlers = {
 *     "list_builder" = "Drupal\registration\RegistrationTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\registration\Form\RegistrationTypeForm",
 *       "edit" = "Drupal\registration\Form\RegistrationTypeForm",
 *       "delete" = "Drupal\registration\Form\RegistrationTypeDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\registration\RegistrationTypeHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "registration_type",
 *   admin_permission = "administer site configuration",
 *   bundle_of = "registration",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/registration_type/{registration_type}",
 *     "add-form" = "/admin/structure/registration_type/add",
 *     "edit-form" = "/admin/structure/registration_type/{registration_type}/edit",
 *     "delete-form" = "/admin/structure/registration_type/{registration_type}/delete",
 *     "collection" = "/admin/structure/registration_type"
 *   }
 * )
 */
class RegistrationType extends ConfigEntityBundleBase implements RegistrationTypeInterface {

  /**
   * The Registration type ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Registration type label.
   *
   * @var string
   */
  protected $label;

}
