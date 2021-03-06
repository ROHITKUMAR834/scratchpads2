<?php

/**
 * Implements hook_dwcarchiver_archives
 */
function dwcarchiver_dwcarchiver_archives()
{
  // Get all of the "taxonomy" vocabularies, and create an archive for each.
  // Note, we may want to be able to allow multiple vocabularies for a single
  // archive.
  $bio_vids = variable_get('biological_vids', array());
  $archives = array();
  // Get the site license URL
  $jurisdiction = variable_get('creative_commons_block_jurisdiction', '');
  if ($jurisdiction && strpos($jurisdiction, '_')) {
    list($jurisdiction, $version) = explode('_', $jurisdiction);
    $version = substr($version, 0, 1) . '.' . substr($version, 1, 1);
  } else {
    $version = CC_DEFAULT_VERSION;
  }
  switch (variable_get('creative_commons_block_licence_type', CC_BY)) {
    // These licence types are text only
    case CC_NONE:
      $url = '';
      break;
    case CC_0:
      $url = 'http://creativecommons.org/about/cc0';
      break;
    case CC_PD:
      $url = 'http://creativecommons.org/about/pdm';
      break;
    case CC_BY:
      $url = CC_LICENCE_URI . '/by/' . $version . '/' . ($jurisdiction ? $jurisdiction : '');
      break;
    case CC_BY_SA:
      $url = CC_LICENCE_URI . '/by-sa/' . $version . '/' . ($jurisdiction ? $jurisdiction : '');
      break;
    case CC_BY_ND:
      $url = CC_LICENCE_URI . '/by-nd/' . $version . '/' . ($jurisdiction ? $jurisdiction : '');
      break;
    case CC_BY_NC:
      $url = CC_LICENCE_URI . '/by-nc/' . $version . '/' . ($jurisdiction ? $jurisdiction : '');
      break;
    case CC_BY_NC_SA:
      $url = CC_LICENCE_URI . '/by-nc-sa/' . $version . '/' . ($jurisdiction ? $jurisdiction : '');
      break;
    case CC_BY_NC_ND:
      $url = CC_LICENCE_URI . '/by-nc-nd/' . $version . '/' . ($jurisdiction ? $jurisdiction : '');
      break;
  }
  // We create the description extension first, as it is a little special.
  $description_extension_gbif = array(
    'name' => 'Description',
    'machine_name' => 'description',
    'rowtype' => 'http://rs.gbif.org/terms/1.0/Description',
    'entity' => 'node',
    'bundles' => array(
      'spm'
    ),
    'field_name' => 'field_taxonomic_name',
    'maps' => array()
  );
  $description_extension_eol = array(
    'name' => 'Description',
    'machine_name' => 'description',
    'rowtype' => 'http://eol.org/schema/media/Document',
    'entity' => 'node',
    'bundles' => array(
      'spm'
    ),
    'field_name' => 'field_taxonomic_name',
    'maps' => array()
  );
  $gbif_replacements = array(
    'general_description' => 'general',
    'behavious' => 'behaviour',
    'conservation_status' => 'conservation',
    'diagnostic_description' => 'diagnostic',
    'life_cycle' => 'lifecycle',
    'life_expectancy' => 'lifespan',
    'population_biology' => 'population',
    'uses' => 'use'
  );
  $eol_replacements = array(
    'behavious' => 'behaviour'
  );
  foreach (array(
    'general_description',
    'morphology',
    'behavious',
    'biology',
    'conservation_status',
    'legislation',
    'management',
    'procedures',
    'threats',
    'associations',
    'cyclicity',
    'diagnostic_description',
    'diseases',
    'dispersal',
    'distribution',
    'ecology',
    'evolution',
    'genetics',
    'growth',
    'habitat',
    'life_cycle',
    'life_expectancy',
    'look_alikes',
    'migration',
    'molecular_biology',
    'physiology',
    'population_biology',
    'reproduction',
    'risk_statement',
    'size',
    'taxon_biology',
    'trophic_strategy',
    'uses'
  ) as $field) {
    $description_extension_gbif['maps'][] = array(
      array(
        'field' => 'uuid',
        'uri' => 'coreid',
        'required' => 0,
        'processor' => 'dwcarchiver_raw_text'
      ),
      array(
        'field' => 'dwcarchiver_text_entry',
        'uri' => 'http://purl.org/dc/terms/type',
        'required' => 0,
        'processor' => 'dwcarchiver_value',
        'value' => (isset($gbif_replacements[$field]) ? $gbif_replacements[$field] : $field)
      ),
      array(
        'field' => 'content_url',
        'uri' => 'http://purl.org/dc/terms/source',
        'required' => 0,
        'processor' => 'dwcarchiver_content_url'
      ),
      array(
        'field' => 'dwcarchiver_text_entry_1',
        'uri' => 'http://purl.org/dc/terms/license',
        'required' => 0,
        'processor' => 'dwcarchiver_value',
        'value' => $url
      ),
      array(
        'field' => 'content_language',
        'uri' => 'http://purl.org/dc/terms/language',
        'required' => 0,
        'processor' => 'dwcarchiver_content_language'
      ),
      array(
        'field' => 'field_' . $field,
        'uri' => 'http://purl.org/dc/terms/description',
        'required' => '1',
        'processor' => 'dwcarchiver_raw_text'
      )
    );
    $description_extension_eol['maps'][] = array(
      array(
        'field' => 'uuid',
        'uri' => 'coreid',
        'required' => 0,
        'processor' => 'dwcarchiver_raw_text'
      ),
      array(
        'field' => 'uuid',
        'uri' => 'http://purl.org/dc/terms/identifier',
        'required' => 0,
        'processor' => 'dwcarchiver_raw_text_concatenate_value_after',
        'value' => '#' . $field
      ),
      array(
        'field' => 'user:uuid',
        'uri' => 'http://eol.org/schema/agent/agentID',
        'required' => 1,
        'processor' => 'dwcarchiver_raw_text'
      ),
      array(
        'field' => 'dwcarchiver_text_entry',
        'uri' => 'http://purl.org/dc/terms/type',
        'required' => 0,
        'processor' => 'dwcarchiver_value',
        'value' => 'http://purl.org/dc/dcmitype/Text'
      ),
      array(
        'field' => 'dwcarchiver_text_entry_1',
        'uri' => 'http://rs.tdwg.org/audubon_core/subtype',
        'required' => 0,
        'processor' => 'dwcarchiver_value',
        'value' => ''
      ),
      array(
        'field' => 'dwcarchiver_text_entry_2',
        'uri' => 'http://purl.org/dc/terms/format',
        'required' => 0,
        'processor' => 'dwcarchiver_value',
        'value' => 'text/html'
      ),
      array(
        'field' => 'dwcarchiver_text_entry_3',
        'uri' => 'http://iptc.org/std/Iptc4xmpExt/1.0/xmlns/CVterm',
        'required' => 0,
        'processor' => 'dwcarchiver_value',
        'value' => 'http://rs.tdwg.org/ontology/voc/SPMInfoItems#' . str_replace('_', '', (isset($eol_replacements[$field]) ? $eol_replacements[$field] : $field))
      ),
      array(
        'field' => 'dwcarchiver_text_entry_4',
        'uri' => 'http://purl.org/dc/terms/title',
        'required' => 0,
        'processor' => 'dwcarchiver_value',
        'value' => ''
      ),
      array(
        'field' => 'field_' . $field,
        'uri' => 'http://purl.org/dc/terms/description',
        'required' => '1',
        'processor' => 'dwcarchiver_raw_text'
      ),
      array(
        'field' => 'content_url',
        'uri' => 'http://rs.tdwg.org/ac/terms/furtherInformationURL',
        'required' => 0,
        'processor' => 'dwcarchiver_content_url'
      ),
      array(
        'field' => 'dwcarchiver_text_entry_5',
        'uri' => 'http://ns.adobe.com/xap/1.0/rights/UsageTerms',
        'required' => 0,
        'processor' => 'dwcarchiver_value',
        'value' => $url
      ),
      array(
        'field' => 'content_language',
        'uri' => 'http://purl.org/dc/terms/language',
        'required' => 0,
        'processor' => 'dwcarchiver_content_language'
      ),
      array(
        'field' => 'user:name',
        'uri' => 'http://ns.adobe.com/xap/1.0/rights/Owner',
        'required' => 0,
        'processor' => 'dwcarchiver_raw_text'
      )
    );
  }
  $bundles = array();
  foreach ($bio_vids as $vid => $type) {
    if ($type) {
      $vocabulary = taxonomy_vocabulary_load($vid);
      if ($vocabulary) {
        $bundles[] = $vocabulary->machine_name;
      }
    }
  }
  if (count($bundles)) {
    if (module_exists('darwincore')) {
      //
      //
      //
      // -- GBIF ---------------------------------------------------------------
      //
      //
      //
      $archives[] = array(
        'name' => 'GBIF DwCA',
        'machine_name' => 'gbif-dwca',
        'entity' => 'taxonomy_term',
        'bundles' => $bundles,
        'rowtype' => 'http://rs.tdwg.org/dwc/terms/Taxon',
        'extensions' => array(
          array(
            'name' => 'Specimen',
            'machine_name' => 'specimen',
            'rowtype' => 'http://rs.tdwg.org/dwc/terms/Occurrence',
            'entity' => 'node',
            'bundles' => array(
              'specimen_observation'
            ),
            'field_name' => 'field_taxonomic_name',
            'maps' => array(
              array(
                array(
                  'field' => 'uuid',
                  'uri' => 'coreid',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'uuid',
                  'uri' => 'http://rs.tdwg.org/dwc/terms/occurrenceID',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'field_basis_of_record',
                  'uri' => 'http://rs.tdwg.org/dwc/terms/basisOfRecord',
                  'required' => 1,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'field_type_status',
                  'uri' => 'http://rs.tdwg.org/dwc/terms/typeStatus',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'field_institution_code',
                  'uri' => 'http://rs.tdwg.org/dwc/terms/institutionCode',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'field_location:field_map',
                  'uri' => 'http://rs.tdwg.org/dwc/terms/decimalLongitude',
                  'required' => 0,
                  'processor' => 'gm3_longitude'
                ),
                array(
                  'field' => 'field_location:field_map',
                  'uri' => 'http://rs.tdwg.org/dwc/terms/decimalLatitude',
                  'required' => 0,
                  'processor' => 'gm3_latitude'
                ),
                array(
                  'field' => 'field_collection_code',
                  'uri' => 'http://rs.tdwg.org/dwc/terms/collectionCode',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'field_catalogue_number',
                  'uri' => 'http://rs.tdwg.org/dwc/terms/catalogNumber',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                )
              )
            )
          ),
          array(
            'name' => 'Reference',
            'machine_name' => 'reference',
            'rowtype' => 'http://rs.gbif.org/terms/1.0/Reference',
            'entity' => 'node',
            'bundles' => array(
              'biblio'
            ),
            'field_name' => 'field_taxonomic_name',
            'maps' => array(
              array(
                array(
                  'field' => 'uuid',
                  'uri' => 'coreid',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'biblio_issn',
                  'uri' => 'http://purl.org/dc/terms/identifier',
                  'required' => 0,
                  'processor' => 'dwcarchiver_biblio'
                ),
                array(
                  'field' => 'biblio_isbn',
                  'uri' => 'http://purl.org/dc/terms/identifier',
                  'required' => 0,
                  'processor' => 'dwcarchiver_biblio'
                ),
                array(
                  'field' => 'biblio_doi',
                  'uri' => 'http://purl.org/dc/terms/identifier',
                  'required' => 0,
                  'processor' => 'dwcarchiver_biblio'
                ),
                array(
                  'field' => 'biblio_type',
                  'uri' => 'http://purl.org/dc/terms/type',
                  'required' => 0,
                  'processor' => 'dwcarchiver_biblio'
                ),
                array(
                  'field' => 'title',
                  'uri' => 'http://purl.org/dc/terms/title',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'content_language',
                  'uri' => 'http://purl.org/dc/terms/language',
                  'required' => 0,
                  'processor' => 'dwcarchiver_content_language'
                ),
                array(
                  'field' => 'content_url',
                  'uri' => 'http://purl.org/dc/terms/identifier',
                  'required' => 0,
                  'processor' => 'dwcarchiver_content_url'
                ),
                array(
                  'field' => 'biblio_date',
                  'uri' => 'http://purl.org/dc/terms/date',
                  'required' => 0,
                  'processor' => 'dwcarchiver_biblio'
                ),
                array(
                  'field' => 'biblio_authors_field',
                  'uri' => 'http://purl.org/dc/terms/creator',
                  'required' => 0,
                  'processor' => 'dwcarchiver_biblio'
                )
              )
            )
          ),
          array(
            'name' => 'Distribution',
            'machine_name' => 'distribution',
            'rowtype' => 'http://rs.gbif.org/terms/1.0/Distribution',
            'entity' => 'node',
            'bundles' => array(
              'specimen_observation'
            ),
            'field_name' => 'field_taxonomic_name',
            'maps' => array(
              array(
                array(
                  'field' => 'uuid',
                  'uri' => 'coreid',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'content_url',
                  'uri' => 'http://rs.tdwg.org/dwc/terms/locationID',
                  'required' => 0,
                  'processor' => 'dwcarchiver_content_url'
                )
              )
            )
          ),
          array(
            'name' => 'Image',
            'machine_name' => 'image',
            'rowtype' => 'http://rs.gbif.org/terms/1.0/Image',
            'entity' => 'file',
            'bundles' => array(
              'image'
            ),
            'field_name' => 'field_taxonomic_name',
            'maps' => array(
              array(
                array(
                  'field' => 'uuid',
                  'uri' => 'coreid',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'file_url',
                  'uri' => 'http://purl.org/dc/terms/identifier',
                  'required' => 0,
                  'processor' => 'dwcarchiver_file_url'
                ),
                array(
                  'field' => 'filename',
                  'uri' => 'http://purl.org/dc/terms/title',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'uuid',
                  'uri' => 'http://purl.org/dc/terms/identifier',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'filemime',
                  'uri' => 'http://purl.org/dc/terms/format',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'field_cc_licence',
                  'uri' => 'http://purl.org/dc/terms/license',
                  'required' => 0,
                  'processor' => 'dwcarchiver_creative_commons_url'
                )
              )
            )
          ),
          $description_extension_gbif,
          array(
            'name' => 'Vernacular Names',
            'machine_name' => 'vernacularnames',
            'rowtype' => 'http://rs.gbif.org/terms/1.0/VernacularName',
            'entity' => 'field_collection_item',
            'bundles' => array(
              'field_vernacular_name_collection'
            ),
            'field_name' => 'field_vernacular_name_collection',
            'maps' => array(
              array(
                array(
                  'field' => 'uuid',
                  'uri' => 'coreid',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'field_vernacular_name',
                  'uri' => 'http://rs.tdwg.org/dwc/terms/vernacularName',
                  'required' => 1,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'field_language',
                  'uri' => 'http://purl.org/dc/terms/language',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'field_vernacular_locality',
                  'uri' => 'http://rs.tdwg.org/dwc/terms/locality',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                ),
                array(
                  'field' => 'field_vernacular_name_remarks',
                  'uri' => 'http://rs.tdwg.org/dwc/terms/taxonRemarks',
                  'required' => 0,
                  'processor' => 'dwcarchiver_raw_text'
                )
              )
            )
          )
        ),
        'maps' => array(
          array(
            array(
              'field' => 'uuid',
              'uri' => 'coreid',
              'required' => 0,
              'processor' => 'dwcarchiver_raw_text'
            ),
            array(
              'field' => 'uuid',
              'uri' => 'http://rs.tdwg.org/dwc/terms/taxonID',
              'required' => 0,
              'processor' => 'dwcarchiver_raw_text'
            ),
            array(
              'field' => 'field_rank',
              'uri' => 'http://rs.tdwg.org/dwc/terms/taxonRank',
              'required' => 0,
              'processor' => 'dwcarchiver_raw_text'
            ),
            array(
              'field' => 'field_unacceptability_reason',
              'uri' => 'http://rs.tdwg.org/dwc/terms/taxonomicStatus',
              'required' => 0,
              'processor' => 'dwcarchiver_raw_text'
            ),
            array(
              'field' => 'field_authors',
              'uri' => 'http://rs.tdwg.org/dwc/terms/scientificNameAuthorship',
              'required' => 0,
              'processor' => 'dwcarchiver_raw_text'
            ),
            array(
              'field' => 'name',
              'uri' => 'http://rs.tdwg.org/dwc/terms/scientificName',
              'required' => 0,
              'processor' => 'dwcarchiver_raw_text'
            ),
            array(
              'field' => 'parent:uuid',
              'uri' => 'http://rs.tdwg.org/dwc/terms/parentNameUsageID',
              'required' => 0,
              'processor' => 'dwcarchiver_raw_text'
            ),
            array(
              'field' => 'field_unacceptability_reason',
              'uri' => 'http://rs.tdwg.org/dwc/terms/nomenclaturalStatus',
              'required' => 0,
              'processor' => 'dwcarchiver_raw_text'
            ),
            array(
              'field' => 'field_reference:content_url',
              'uri' => 'http://rs.tdwg.org/dwc/terms/namePublishedInID',
              'required' => 0,
              'processor' => 'dwcarchiver_content_url'
            ),
            array(
              'field' => 'field_reference:title',
              'uri' => 'http://rs.tdwg.org/dwc/terms/namePublishedIn',
              'required' => 0,
              'processor' => 'dwcarchiver_raw_text'
            ),
            array(
              'field' => "field_aan_$vid:uuid",
              'uri' => 'http://rs.tdwg.org/dwc/terms/acceptedNameUsageID',
              'required' => 0,
              'processor' => 'dwcarchiver_raw_text'
            )
          )
        )
      );
    }
    //
    //
    //
    // -- EOL ----------------------------------------------------------------
    //
    //
    //
    $archives[] = array(
      'name' => 'EOL DwCA',
      'machine_name' => 'eol-dwca',
      'entity' => 'taxonomy_term',
      'bundles' => $bundles,
      'rowtype' => 'http://rs.tdwg.org/dwc/terms/Taxon',
      'extensions' => array(
        array(
          'name' => 'Reference',
          'machine_name' => 'reference',
          'rowtype' => 'http://eol.org/schema/reference/Reference',
          'entity' => 'node',
          'bundles' => array(
            'biblio'
          ),
          'field_name' => 'field_taxonomic_name',
          'maps' => array(
            array(
              array(
                'field' => 'dwcarchiver_text_entry',
                'uri' => 'coreid',
                'required' => 0,
                'processor' => 'dwcarchiver_value',
                'value' => ''
              ),
              array(
                'field' => 'uuid',
                'uri' => 'http://purl.org/dc/terms/identifier',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'content_url',
                'uri' => 'http://purl.org/ontology/bibo/uri',
                'required' => 0,
                'processor' => 'dwcarchiver_content_url'
              ),
              array(
                'field' => 'citation',
                'uri' => 'http://eol.org/schema/reference/full_reference',
                'required' => 0,
                'processor' => 'dwcarchiver_biblio'
              )
            )
          )
        ),
        array(
          'name' => 'Agents',
          'machine_name' => 'agents',
          'rowtype' => 'http://eol.org/schema/agent/Agent',
          'entity' => 'user',
          'bundles' => array(
            'user'
          ),
          'field_name' => '',
          'maps' => array(
            array(
              array(
                'field' => 'dwcarchiver_text_entry',
                'uri' => 'coreid',
                'required' => 0,
                'processor' => 'dwcarchiver_value',
                'value' => ''
              ),
              array(
                'field' => 'uuid',
                'uri' => 'http://purl.org/dc/terms/identifier',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'field_user_given_names',
                'uri' => 'http://xmlns.com/foaf/spec/#term_givenName',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'field_user_family_name',
                'uri' => 'http://xmlns.com/foaf/spec/#term_familyName',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'field_homepage',
                'uri' => 'http://xmlns.com/foaf/spec/#term_homepage',
                'required' => 0,
                'processor' => 'dwcarchiver_link'
              ),
              array(
                'field' => 'field_user_institution',
                'uri' => 'http://eol.org/schema/agent/organization',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'name',
                'uri' => 'http://xmlns.com/foaf/spec/#term_name',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'dwcarchiver_text_entry_1',
                'uri' => 'http://eol.org/schema/agent/agentRole',
                'required' => 0,
                'processor' => 'dwcarchiver_value',
                'value' => 'Author'
              )
            )
          )
        ),
        array(
          'name' => 'Media',
          'machine_name' => 'media',
          'rowtype' => 'http://eol.org/schema/media/Document',
          'entity' => 'file',
          'bundles' => array(
            'image'
          ),
          'field_name' => 'field_taxonomic_name',
          'maps' => array(
            array(
              array(
                'field' => 'uuid',
                'uri' => 'coreid',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'uuid',
                'uri' => 'http://purl.org/dc/terms/identifier',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'filemime',
                'uri' => 'http://purl.org/dc/terms/type',
                'required' => 0,
                'processor' => 'dwcarchiver_mime_to_dcmitype'
              ),
              array(
                'field' => 'dwcarchiver_text_entry',
                'uri' => 'http://rs.tdwg.org/audubon_core/subtype',
                'required' => 0,
                'processor' => 'dwcarchiver_value',
                'value' => ''
              ),
              array(
                'field' => 'filemime',
                'uri' => 'http://purl.org/dc/terms/format',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'filename',
                'uri' => 'http://purl.org/dc/terms/title',
                'required' => 1,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'field_description',
                'uri' => 'http://purl.org/dc/terms/description',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'file_url',
                'uri' => 'http://rs.tdwg.org/ac/terms/accessURI',
                'required' => 1,
                'processor' => 'dwcarchiver_file_url'
              ),
              array(
                'field' => 'content_url',
                'uri' => 'http://rs.tdwg.org/ac/terms/furtherInformationURL',
                'required' => 0,
                'processor' => 'dwcarchiver_content_url'
              ),
              array(
                'field' => 'timestamp',
                'uri' => 'http://ns.adobe.com/xap/1.0/CreateDate',
                'required' => 0,
                'processor' => 'dwcarchiver_timestamp'
              ),
              array(
                'field' => 'field_cc_licence',
                'uri' => 'http://ns.adobe.com/xap/1.0/rights/UsageTerms',
                'required' => 0,
                'processor' => 'dwcarchiver_creative_commons_url'
              ),
              array(
                'field' => 'field_creator',
                'uri' => 'http://ns.adobe.com/xap/1.0/rights/Owner',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              )
            )
          )
        ),
        $description_extension_eol,
        array(
          'name' => 'Vernacular Names',
          'machine_name' => 'vernacularnames',
          'rowtype' => 'http://rs.gbif.org/terms/1.0/VernacularName',
          'entity' => 'field_collection_item',
          'bundles' => array(
            'field_vernacular_name_collection'
          ),
          'field_name' => 'field_vernacular_name_collection',
          'maps' => array(
            array(
              array(
                'field' => 'uuid',
                'uri' => 'coreid',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'field_vernacular_name',
                'uri' => 'http://rs.tdwg.org/dwc/terms/vernacularName',
                'required' => 1,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'field_reference:content_url',
                'uri' => 'http://purl.org/dc/terms/source',
                'required' => 0,
                'processor' => 'dwcarchiver_content_url'
              ),
              array(
                'field' => 'field_language',
                'uri' => 'http://purl.org/dc/terms/language',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'field_vernacular_locality',
                'uri' => 'http://rs.tdwg.org/dwc/terms/locality',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              ),
              array(
                'field' => 'field_vernacular_name_remarks',
                'uri' => 'http://rs.tdwg.org/dwc/terms/taxonRemarks',
                'required' => 0,
                'processor' => 'dwcarchiver_raw_text'
              )
            )
          )
        )
      ),
      'maps' => array(
        array(
          array(
            'field' => 'uuid',
            'uri' => 'coreid',
            'required' => 0,
            'processor' => 'dwcarchiver_raw_text'
          ),
          array(
            'field' => 'uuid',
            'uri' => 'http://rs.tdwg.org/dwc/terms/taxonID',
            'required' => 0,
            'processor' => 'dwcarchiver_raw_text'
          ),
          array(
            'field' => 'name',
            'uri' => 'http://rs.tdwg.org/dwc/terms/scientificName',
            'required' => 0,
            'processor' => 'dwcarchiver_raw_text'
          ),
          array(
            'field' => 'parent:uuid',
            'uri' => 'http://rs.tdwg.org/dwc/terms/parentNameUsageID',
            'required' => 0,
            'processor' => 'dwcarchiver_raw_text'
          ),
          array(
            'field' => 'field_rank',
            'uri' => 'http://rs.tdwg.org/dwc/terms/taxonRank',
            'required' => 0,
            'processor' => 'dwcarchiver_raw_text'
          ),
          array(
            'field' => 'content_url',
            'uri' => 'http://rs.tdwg.org/ac/terms/furtherInformationURL',
            'required' => 0,
            'processor' => 'dwcarchiver_content_url'
          ),
          array(
            'field' => 'field_unacceptability_reason',
            'uri' => 'http://rs.tdwg.org/dwc/terms/taxonomicStatus',
            'required' => 0,
            'processor' => 'dwcarchiver_raw_text'
          ),
          array(
            'field' => 'description',
            'uri' => 'http://rs.tdwg.org/dwc/terms/taxonRemarks',
            'required' => 0,
            'processor' => 'dwcarchiver_raw_text'
          ),
          array(
            'field' => 'field_reference:title',
            'uri' => 'http://rs.tdwg.org/dwc/terms/namePublishedIn',
            'required' => 0,
            'processor' => 'dwcarchiver_raw_text'
          ),
          array(
            'field' => 'field_reference:uuid',
            'uri' => 'http://eol.org/schema/media/referenceID',
            'required' => 0,
            'processor' => 'dwcarchiver_raw_text'
          ),
          array(
            'field' => 'special_biblio_back_reference:uuid',
            'uri' => 'http://eol.org/schema/media/referenceID',
            'required' => 0,
            'processor' => 'dwcarchiver_raw_text_group_concat'
          )
        )
      )
    );
  }
  return $archives;
}
