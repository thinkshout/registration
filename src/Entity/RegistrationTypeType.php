<?php

namespace Drupal\registration\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;

/**
 * Defines the Registration type type entity.
 *
 * @ConfigEntityType(
 *   id = "registration_type_type",
 *   label = @Translation("Registration type type"),
 *   handlers = {
 *     "list_builder" = "Drupal\registration\RegistrationTypeTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\registration\Form\RegistrationTypeTypeForm",
 *       "edit" = "Drupal\registration\Form\RegistrationTypeTypeForm",
 *       "delete" = "Drupal\registration\Form\RegistrationTypeTypeDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\registration\RegistrationTypeTypeHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "registration_type_type",
 *   admin_permission = "administer site configuration",
 *   bundle_of = "registration_type",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/registration_type_type/{registration_type_type}",
 *     "add-form" = "/admin/structure/registration_type_type/add",
 *     "edit-form" = "/admin/structure/registration_type_type/{registration_type_type}/edit",
 *     "delete-form" = "/admin/structure/registration_type_type/{registration_type_type}/delete",
 *     "collection" = "/admin/structure/registration_type_type"
 *   }
 * )
 */
class RegistrationTypeType extends ConfigEntityBundleBase implements RegistrationTypeTypeInterface {

  /**
   * The Registration type type ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Registration type type label.
   *
   * @var string
   */
  protected $label;

}
