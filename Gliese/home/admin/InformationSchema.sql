--###################################################################################
--# build views of INFORMATION_SCHEMA
--###################################################################################

--##############################################################
--# SYS AUTHORIZATION
--##############################################################

SET SESSION AUTHORIZATION SYS;

--##############################################################
--# INFORMATION_SCHEMA.WHOLE_TABLES
--# internal use only
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.WHOLE_TABLES;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.WHOLE_TABLES
(
       OWNER_ID
     , SCHEMA_ID
     , TABLE_ID
     , TABLESPACE_ID
     , TABLE_NAME
     , TABLE_TYPE
     , TABLE_TYPE_ID
     , SYSTEM_VERSION_START_COLUMN_NAME
     , SYSTEM_VERSION_END_COLUMN_NAME
     , SYSTEM_VERSION_RETENTION_PERIOD
     , SELF_REFERENCING_COLUMN_NAME
     , REFERENCE_GENERATION
     , USER_DEFINED_TYPE_CATALOG
     , USER_DEFINED_TYPE_SCHEMA
     , USER_DEFINED_TYPE_NAME
     , IS_INSERTABLE_INTO
     , IS_TYPED
     , COMMIT_ACTION
     , IS_SET_SUPPLOG_PK
     , CREATED_TIME
     , MODIFIED_TIME
     , COMMENTS
)
AS
SELECT 
       OWNER_ID
     , SCHEMA_ID
     , TABLE_ID
     , TABLESPACE_ID
     , TABLE_NAME
     , TABLE_TYPE
     , TABLE_TYPE_ID
     , SYSTEM_VERSION_START_COLUMN_NAME
     , SYSTEM_VERSION_END_COLUMN_NAME
     , SYSTEM_VERSION_RETENTION_PERIOD
     , SELF_REFERENCING_COLUMN_NAME
     , REFERENCE_GENERATION
     , USER_DEFINED_TYPE_CATALOG
     , USER_DEFINED_TYPE_SCHEMA
     , USER_DEFINED_TYPE_NAME
     , IS_INSERTABLE_INTO
     , IS_TYPED
     , COMMIT_ACTION
     , IS_SET_SUPPLOG_PK
     , CREATED_TIME
     , MODIFIED_TIME
     , COMMENTS
  FROM DEFINITION_SCHEMA.TABLES
 WHERE TABLE_TYPE <> 'SEQUENCE'
 UNION ALL
SELECT 
       ( SELECT OWNER_ID FROM DEFINITION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = 'FIXED_TABLE_SCHEMA' ) -- OWNER_ID
     , ( SELECT SCHEMA_ID FROM DEFINITION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = 'FIXED_TABLE_SCHEMA' ) -- SCHEMA_ID
     , TABLE_ID
     , NULL -- TABLESPACE_ID
     , TABLE_NAME
     , USAGE_TYPE -- TABLE_TYPE
     , DECODE( USAGE_TYPE, 'FIXED TABLE', 7, 8 )  -- TABLE_TYPE_ID  
     , NULL -- SYSTEM_VERSION_START_COLUMN_NAME
     , NULL -- SYSTEM_VERSION_END_COLUMN_NAME
     , NULL -- SYSTEM_VERSION_RETENTION_PERIOD
     , NULL -- SELF_REFERENCING_COLUMN_NAME
     , NULL -- REFERENCE_GENERATION
     , NULL -- USER_DEFINED_TYPE_CATALOG
     , NULL -- USER_DEFINED_TYPE_SCHEMA
     , NULL -- USER_DEFINED_TYPE_NAME
     , FALSE -- IS_INSERTABLE_INTO
     , FALSE -- IS_TYPED 
     , NULL -- IS_TYPED
     , FALSE -- IS_SET_SUPPLOG_PK
     , NULL -- CREATED_TIME
     , NULL -- MODIFIED_TIME
     , COMMENTS
  FROM FIXED_TABLE_SCHEMA.X$TABLES;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.WHOLE_TABLES
        IS 'internal use only';

--#####################
--# comment column
--#####################


--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.WHOLE_TABLES TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.WHOLE_COLUMNS
--# internal use only
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.WHOLE_COLUMNS;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.WHOLE_COLUMNS
(
       OWNER_ID
     , SCHEMA_ID
     , TABLE_ID
     , COLUMN_ID
     , COLUMN_NAME
     , PHYSICAL_ORDINAL_POSITION
     , LOGICAL_ORDINAL_POSITION
     , DTD_IDENTIFIER
     , DOMAIN_CATALOG
     , DOMAIN_SCHEMA 
     , DOMAIN_NAME   
     , COLUMN_DEFAULT  
     , IS_NULLABLE     
     , IS_SELF_REFERENCING 
     , IS_IDENTITY         
     , IDENTITY_GENERATION 
     , IDENTITY_GENERATION_ID 
     , IDENTITY_START     
     , IDENTITY_INCREMENT 
     , IDENTITY_MAXIMUM   
     , IDENTITY_MINIMUM   
     , IDENTITY_CYCLE     
     , IDENTITY_TABLESPACE_ID 
     , IDENTITY_PHYSICAL_ID   
     , IDENTITY_CACHE_SIZE    
     , IS_GENERATED           
     , IS_SYSTEM_VERSION_START  
     , IS_SYSTEM_VERSION_END    
     , SYSTEM_VERSION_TIMESTAMP_GENERATION 
     , IS_UPDATABLE                        
     , IS_UNUSED                           
     , COMMENTS        
)
AS
SELECT 
       OWNER_ID
     , SCHEMA_ID
     , TABLE_ID
     , COLUMN_ID
     , COLUMN_NAME
     , PHYSICAL_ORDINAL_POSITION
     , LOGICAL_ORDINAL_POSITION
     , DTD_IDENTIFIER
     , DOMAIN_CATALOG
     , DOMAIN_SCHEMA 
     , DOMAIN_NAME   
     , COLUMN_DEFAULT  
     , IS_NULLABLE     
     , IS_SELF_REFERENCING 
     , IS_IDENTITY         
     , IDENTITY_GENERATION 
     , IDENTITY_GENERATION_ID 
     , IDENTITY_START     
     , IDENTITY_INCREMENT 
     , IDENTITY_MAXIMUM   
     , IDENTITY_MINIMUM   
     , IDENTITY_CYCLE     
     , IDENTITY_TABLESPACE_ID 
     , IDENTITY_PHYSICAL_ID   
     , IDENTITY_CACHE_SIZE    
     , IS_GENERATED           
     , IS_SYSTEM_VERSION_START  
     , IS_SYSTEM_VERSION_END    
     , SYSTEM_VERSION_TIMESTAMP_GENERATION 
     , IS_UPDATABLE                        
     , IS_UNUSED      
     , COMMENTS        
  FROM DEFINITION_SCHEMA.COLUMNS
 UNION ALL
SELECT 
       ( SELECT OWNER_ID FROM DEFINITION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = 'FIXED_TABLE_SCHEMA' ) -- OWNER_ID
     , ( SELECT SCHEMA_ID FROM DEFINITION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = 'FIXED_TABLE_SCHEMA' ) -- SCHEMA_ID
     , TABLE_ID
     , COLUMN_ID
     , COLUMN_NAME
     , ORDINAL_POSITION
     , ORDINAL_POSITION
     , COLUMN_ID -- DTD_IDENTIFIER
     , NULL -- DOMAIN_CATALOG
     , NULL -- DOMAIN_SCHEMA 
     , NULL -- DOMAIN_NAME   
     , NULL -- COLUMN_DEFAULT  
     , FALSE -- IS_NULLABLE     
     , FALSE -- IS_SELF_REFERENCING 
     , FALSE -- IS_IDENTITY         
     , NULL -- IDENTITY_GENERATION 
     , NULL -- IDENTITY_GENERATION_ID 
     , NULL -- IDENTITY_START     
     , NULL -- IDENTITY_INCREMENT 
     , NULL -- IDENTITY_MAXIMUM   
     , NULL -- IDENTITY_MINIMUM   
     , NULL -- IDENTITY_CYCLE     
     , NULL -- IDENTITY_TABLESPACE_ID 
     , NULL -- IDENTITY_PHYSICAL_ID   
     , NULL -- IDENTITY_CACHE_SIZE    
     , FALSE -- IS_GENERATED           
     , FALSE -- IS_SYSTEM_VERSION_START  
     , FALSE -- IS_SYSTEM_VERSION_END    
     , NULL -- SYSTEM_VERSION_TIMESTAMP_GENERATION 
     , FALSE -- IS_UPDATABLE                        
     , FALSE -- IS_UNUSED                           
     , COMMENTS        
  FROM FIXED_TABLE_SCHEMA.X$COLUMNS
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.WHOLE_COLUMNS
        IS 'internal use only';

--#####################
--# comment column
--#####################


--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.WHOLE_COLUMNS TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.WHOLE_DTDS
--# internal use only
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.WHOLE_DTDS;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.WHOLE_DTDS
(
       OWNER_ID
     , SCHEMA_ID
     , TABLE_ID
     , COLUMN_ID
     , OBJECT_CATALOG
     , OBJECT_SCHEMA
     , OBJECT_NAME
     , OBJECT_TYPE
     , DTD_IDENTIFIER
     , DATA_TYPE 
     , DATA_TYPE_ID
     , CHARACTER_SET_CATALOG
     , CHARACTER_SET_SCHEMA
     , CHARACTER_SET_NAME
     , STRING_LENGTH_UNIT
     , STRING_LENGTH_UNIT_ID
     , CHARACTER_MAXIMUM_LENGTH
     , CHARACTER_OCTET_LENGTH
     , COLLATION_CATALOG
     , COLLATION_SCHEMA 
     , COLLATION_NAME 
     , NUMERIC_PRECISION 
     , NUMERIC_PRECISION_RADIX
     , NUMERIC_SCALE 
     , DECLARED_DATA_TYPE
     , DECLARED_NUMERIC_PRECISION
     , DECLARED_NUMERIC_SCALE
     , DATETIME_PRECISION
     , INTERVAL_TYPE
     , INTERVAL_TYPE_ID
     , INTERVAL_PRECISION
     , USER_DEFINED_TYPE_CATALOG
     , USER_DEFINED_TYPE_SCHEMA
     , USER_DEFINED_TYPE_NAME
     , SCOPE_CATALOG
     , SCOPE_SCHEMA
     , SCOPE_NAME
     , MAXIMUM_CARDINALITY
     , PHYSICAL_MAXIMUM_LENGTH
)
AS
SELECT
       OWNER_ID
     , SCHEMA_ID
     , TABLE_ID
     , COLUMN_ID
     , OBJECT_CATALOG
     , OBJECT_SCHEMA
     , OBJECT_NAME
     , OBJECT_TYPE
     , DTD_IDENTIFIER
     , CAST( CASE WHEN DATA_TYPE IN ( 'INTERVAL YEAR TO MONTH', 'INTERVAL DAY TO SECOND' )
                       THEN 'INTERVAL ' || INTERVAL_TYPE 
                  WHEN ( DATA_TYPE = 'NUMBER' AND NUMERIC_PRECISION_RADIX = 2 )
                       THEN 'FLOAT'
                  ELSE DATA_TYPE
                  END
             AS VARCHAR(128 OCTETS) ) -- DATA_TYPE
     , DATA_TYPE_ID
     , CHARACTER_SET_CATALOG
     , CHARACTER_SET_SCHEMA
     , CHARACTER_SET_NAME
     , STRING_LENGTH_UNIT
     , STRING_LENGTH_UNIT_ID
     , CHARACTER_MAXIMUM_LENGTH
     , CHARACTER_OCTET_LENGTH
     , COLLATION_CATALOG
     , COLLATION_SCHEMA 
     , COLLATION_NAME 
     , NUMERIC_PRECISION 
     , NUMERIC_PRECISION_RADIX
     , CAST( CASE WHEN NUMERIC_SCALE BETWEEN -256 AND 256
                  THEN NUMERIC_SCALE
                  ELSE NULL
                  END 
             AS NUMBER ) 
     , DECLARED_DATA_TYPE
     , DECLARED_NUMERIC_PRECISION
     , DECLARED_NUMERIC_SCALE
     , DATETIME_PRECISION
     , INTERVAL_TYPE
     , INTERVAL_TYPE_ID
     , INTERVAL_PRECISION
     , USER_DEFINED_TYPE_CATALOG
     , USER_DEFINED_TYPE_SCHEMA
     , USER_DEFINED_TYPE_NAME
     , SCOPE_CATALOG
     , SCOPE_SCHEMA
     , SCOPE_NAME
     , MAXIMUM_CARDINALITY
     , PHYSICAL_MAXIMUM_LENGTH
  FROM DEFINITION_SCHEMA.DATA_TYPE_DESCRIPTOR
 UNION ALL
SELECT
       ( SELECT OWNER_ID FROM DEFINITION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = 'FIXED_TABLE_SCHEMA' ) -- OWNER_ID
     , ( SELECT SCHEMA_ID FROM DEFINITION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = 'FIXED_TABLE_SCHEMA' ) -- SCHEMA_ID
     , TABLE_ID
     , COLUMN_ID
     , NULL -- OBJECT_CATALOG
     , NULL -- OBJECT_SCHEMA
     , NULL -- OBJECT_NAME
     , NULL -- OBJECT_TYPE
     , COLUMN_ID -- DTD_IDENTIFIER
     , DATA_TYPE 
     , DATA_TYPE_ID
     , NULL -- CHARACTER_SET_CATALOG
     , NULL -- CHARACTER_SET_SCHEMA
     , NULL -- CHARACTER_SET_NAME
     , DECODE( DATA_TYPE, 'CHARACTER VARYING', 'OCTETS', NULL ) -- STRING_LENGTH_UNIT
     , DECODE( DATA_TYPE, 'CHARACTER VARYING', 2, NULL ) -- STRING_LENGTH_UNIT_ID
     , DECODE( DATA_TYPE, 'CHARACTER VARYING', 128, NULL ) -- CHARACTER_MAXIMUM_LENGTH
     , DECODE( DATA_TYPE, 'CHARACTER VARYING', 128, NULL ) -- CHARACTER_OCTET_LENGTH
     , NULL -- COLLATION_CATALOG
     , NULL -- COLLATION_SCHEMA 
     , NULL -- COLLATION_NAME 
     , DECODE( DATA_TYPE, 'NATIVE_SMALLINT', 16, 'NATIVE_INTEGER', 32, 'NATIVE_BIGINT', 64, 'NATIVE_REAL', 24, 'NATIVE_DOUBLE', 53, NULL ) -- NUMERIC_PRECISION 
     , DECODE( DATA_TYPE, 'NATIVE_SMALLINT', 2, 'NATIVE_INTEGER', 2, 'NATIVE_BIGINT', 2, 'NATIVE_REAL', 2, 'NATIVE_DOUBLE', 2, NULL ) -- NUMERIC_PRECISION_RADIX
     , DECODE( DATA_TYPE, 'NATIVE_SMALLINT', 0, 'NATIVE_INTEGER', 0, 'NATIVE_BIGINT', 0, 'NATIVE_REAL', NULL, 'NATIVE_DOUBLE', NULL, NULL ) -- NUMERIC_SCALE 
     , DATA_TYPE -- DECLARED_DATA_TYPE
     , DECODE( DATA_TYPE, 'CHARACTER VARYING', COLUMN_LENGTH, NULL ) -- DECLARED_NUMERIC_PRECISION
     , NULL -- DECLARED_NUMERIC_SCALE
     , DECODE( DATA_TYPE, 'TIMESTAMP WITHOUT TIME ZONE', 6, NULL ) -- DATETIME_PRECISION
     , NULL -- INTERVAL_TYPE
     , NULL -- INTERVAL_TYPE_ID
     , NULL -- INTERVAL_PRECISION
     , NULL -- USER_DEFINED_TYPE_CATALOG
     , NULL -- USER_DEFINED_TYPE_SCHEMA
     , NULL -- USER_DEFINED_TYPE_NAME
     , NULL -- SCOPE_CATALOG
     , NULL -- SCOPE_SCHEMA
     , NULL -- SCOPE_NAME
     , NULL -- MAXIMUM_CARDINALITY
     , COLUMN_LENGTH  -- PHYSICAL_MAXIMUM_LENGTH
  FROM FIXED_TABLE_SCHEMA.X$COLUMNS
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.WHOLE_DTDS
        IS 'internal use only';

--#####################
--# comment column
--#####################


--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.WHOLE_DTDS TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.COLUMNS
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.COLUMNS;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.COLUMNS 
( 
       TABLE_CATALOG 
     , TABLE_OWNER 
     , TABLE_SCHEMA 
     , TABLE_NAME 
     , COLUMN_NAME 
     , ORDINAL_POSITION 
     , COLUMN_DEFAULT 
     , IS_NULLABLE 
     , DATA_TYPE 
     , CHARACTER_MAXIMUM_LENGTH 
     , CHARACTER_OCTET_LENGTH 
     , NUMERIC_PRECISION 
     , NUMERIC_PRECISION_RADIX 
     , NUMERIC_SCALE 
     , DATETIME_PRECISION 
     , INTERVAL_TYPE 
     , INTERVAL_PRECISION 
     , CHARACTER_SET_CATALOG 
     , CHARACTER_SET_SCHEMA 
     , CHARACTER_SET_NAME 
     , COLLATION_CATALOG 
     , COLLATION_SCHEMA 
     , COLLATION_NAME 
     , DOMAIN_CATALOG 
     , DOMAIN_SCHEMA 
     , DOMAIN_NAME 
     , UDT_CATALOG 
     , UDT_SCHEMA 
     , UDT_NAME 
     , SCOPE_CATALOG 
     , SCOPE_SCHEMA 
     , SCOPE_NAME 
     , MAXIMUM_CARDINALITY 
     , DTD_IDENTIFIER 
     , IS_SELF_REFERENCING 
     , IS_IDENTITY 
     , IDENTITY_GENERATION 
     , IDENTITY_START 
     , IDENTITY_INCREMENT 
     , IDENTITY_MAXIMUM 
     , IDENTITY_MINIMUM 
     , IDENTITY_CYCLE 
     , IS_GENERATED 
     , GENERATION_EXPRESSION 
     , IS_SYSTEM_VERSION_START 
     , IS_SYSTEM_VERSION_END 
     , SYSTEM_VERSION_TIMESTAMP_GENERATION 
     , IS_UPDATABLE 
     , DECLARED_DATA_TYPE 
     , DECLARED_NUMERIC_PRECISION 
     , DECLARED_NUMERIC_SCALE 
     , COMMENTS 
) 
AS 
SELECT 
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth.AUTHORIZATION_NAME 
     , sch.SCHEMA_NAME 
     , tab.TABLE_NAME 
     , col.COLUMN_NAME 
     , CAST( col.LOGICAL_ORDINAL_POSITION AS NUMBER ) 
     , col.COLUMN_DEFAULT 
     , col.IS_NULLABLE 
     , CAST( dtd.DATA_TYPE AS VARCHAR(128 OCTETS) )
     , CAST( dtd.CHARACTER_MAXIMUM_LENGTH AS NUMBER ) 
     , CAST( dtd.CHARACTER_OCTET_LENGTH AS NUMBER ) 
     , CAST( dtd.NUMERIC_PRECISION AS NUMBER ) 
     , CAST( dtd.NUMERIC_PRECISION_RADIX AS NUMBER ) 
     , CAST( dtd.NUMERIC_SCALE AS NUMBER ) 
     , CAST( dtd.DATETIME_PRECISION AS NUMBER ) 
     , dtd.INTERVAL_TYPE 
     , CAST( dtd.INTERVAL_PRECISION AS NUMBER ) 
     , dtd.CHARACTER_SET_CATALOG 
     , dtd.CHARACTER_SET_SCHEMA 
     , dtd.CHARACTER_SET_NAME 
     , dtd.COLLATION_CATALOG 
     , dtd.COLLATION_SCHEMA 
     , dtd.COLLATION_NAME 
     , col.DOMAIN_CATALOG 
     , col.DOMAIN_SCHEMA 
     , col.DOMAIN_NAME 
     , dtd.USER_DEFINED_TYPE_CATALOG 
     , dtd.USER_DEFINED_TYPE_SCHEMA 
     , dtd.USER_DEFINED_TYPE_NAME 
     , dtd.SCOPE_CATALOG 
     , dtd.SCOPE_SCHEMA 
     , dtd.SCOPE_NAME 
     , CAST( dtd.MAXIMUM_CARDINALITY AS NUMBER ) 
     , CAST( dtd.DTD_IDENTIFIER AS NUMBER ) 
     , col.IS_SELF_REFERENCING 
     , col.IS_IDENTITY 
     , col.IDENTITY_GENERATION 
     , CAST( col.IDENTITY_START AS NUMBER ) 
     , CAST( col.IDENTITY_INCREMENT AS NUMBER ) 
     , CAST( col.IDENTITY_MAXIMUM AS NUMBER ) 
     , CAST( col.IDENTITY_MINIMUM AS NUMBER ) 
     , col.IDENTITY_CYCLE 
     , col.IS_GENERATED 
     , CAST( NULL AS VARCHAR(128 OCTETS) ) AS GENERATION_EXPRESSION 
     , col.IS_SYSTEM_VERSION_START 
     , col.IS_SYSTEM_VERSION_END 
     , col.SYSTEM_VERSION_TIMESTAMP_GENERATION 
     , col.IS_UPDATABLE 
     , dtd.DECLARED_DATA_TYPE 
     , CAST( dtd.DECLARED_NUMERIC_PRECISION AS NUMBER ) 
     , CAST( dtd.DECLARED_NUMERIC_SCALE AS NUMBER ) 
     , col.COMMENTS 
  FROM 
       INFORMATION_SCHEMA.WHOLE_COLUMNS       AS col 
     , INFORMATION_SCHEMA.WHOLE_DTDS          AS dtd 
     , INFORMATION_SCHEMA.WHOLE_TABLES        AS tab 
     , DEFINITION_SCHEMA.SCHEMATA             AS sch 
     , DEFINITION_SCHEMA.AUTHORIZATIONS       AS auth 
 WHERE  
       col.IS_UNUSED = FALSE
   AND col.DTD_IDENTIFIER = dtd.DTD_IDENTIFIER 
   AND col.TABLE_ID       = tab.TABLE_ID 
   AND col.SCHEMA_ID      = sch.SCHEMA_ID 
   AND col.OWNER_ID       = auth.AUTH_ID 
   AND ( col.COLUMN_ID IN ( SELECT pvcol.COLUMN_ID 
                              FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES AS pvcol 
                             WHERE ( pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS aucol 
                                                            WHERE aucol.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                         ) 
                                  -- OR  
                                  -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                   )
                          ) 
         OR 
         col.TABLE_ID IN ( SELECT pvtab.TABLE_ID 
                             FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES AS pvtab 
                            WHERE ( pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS autab 
                                                           WHERE autab.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                        ) 
                                 -- OR  
                                 -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                 --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                  )  
                          ) 
         OR 
         col.SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID 
                              FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch 
                             WHERE pvsch.PRIVILEGE_TYPE IN ( 'CONTROL SCHEMA', 'SELECT TABLE', 'INSERT TABLE', 'DELETE TABLE', 'UPDATE TABLE', 'LOCK TABLE' ) 
                               AND ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch 
                                                            WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                         ) 
                                  -- OR  
                                  -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                   )  
                          ) 
         OR 
         EXISTS ( SELECT GRANTEE_ID  
                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba 
                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'SELECT ANY TABLE', 'INSERT ANY TABLE', 'DELETE ANY TABLE', 'UPDATE ANY TABLE', 'LOCK ANY TABLE' ) 
                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba 
                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                               ) 
                        -- OR  
                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                         )  
                ) 
       ) 
ORDER BY 
      col.SCHEMA_ID 
    , col.TABLE_ID 
    , col.PHYSICAL_ORDINAL_POSITION
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.COLUMNS 
        IS 'Identify the columns of tables defined in this cataog that are accessible to given user or role.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.TABLE_CATALOG                    
        IS 'catalog name of the column';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.TABLE_OWNER                      
        IS 'owner name of the column'; 
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.TABLE_SCHEMA                     
        IS 'schema name of the column'; 
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.TABLE_NAME                       
        IS 'table name of the column'; 
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.COLUMN_NAME                      
        IS 'column name';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.ORDINAL_POSITION                 
        IS 'the ordinal position (> 0) of the column in the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.COLUMN_DEFAULT                   
        IS 'the default for the column'; 
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IS_NULLABLE                      
        IS 'is nullable of the column'; 
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.DATA_TYPE                        
        IS 'the standard name of the data type'; 
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.CHARACTER_MAXIMUM_LENGTH         
        IS 'the maximum length in characters';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.CHARACTER_OCTET_LENGTH           
        IS 'the maximum length in octets';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.NUMERIC_PRECISION                
        IS 'the numeric precision of the numerical data type'; 
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.NUMERIC_PRECISION_RADIX          
        IS 'the radix ( 2 or 10 ) of the precision of the numerical data type';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.NUMERIC_SCALE                    
        IS 'the numeric scale of the exact numerical data type';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.DATETIME_PRECISION               
        IS 'for a datetime or interval type, the value is the fractional seconds precision';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.INTERVAL_TYPE                    
        IS 'for a interval type, the value is in ( YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, YEAR TO MONTH, DAY TO HOUR, DAY TO MINUTE, DAY TO SECOND, HOUR TO MINUTE, HOUR TO SECOND, MINUTE TO SECOND )';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.INTERVAL_PRECISION               
        IS 'for a interval type, the value is the leading precision';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.CHARACTER_SET_CATALOG            
        IS 'catalog name of the character set if is is a character string type';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.CHARACTER_SET_SCHEMA             
        IS 'schema name of the character set if is is a character string type';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.CHARACTER_SET_NAME               
        IS 'character set name of the character set if is is a character string type';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.COLLATION_CATALOG                
        IS 'catalog name of the applicable collation if is is a character string type';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.COLLATION_SCHEMA                 
        IS 'schema name of the applicable collation if is is a character string type';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.COLLATION_NAME                   
        IS 'collation name of the applicable collation if is is a character string type';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.DOMAIN_CATALOG                   
        IS 'catalog name of the domain used by the column being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.DOMAIN_SCHEMA                    
        IS 'schema name of the domain used by the column being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.DOMAIN_NAME                      
        IS 'domain name of the domain used by the column being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.UDT_CATALOG                      
        IS 'catalog name of the user-defined type of the data type being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.UDT_SCHEMA                       
        IS 'schema name of the user-defined type of the data type being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.UDT_NAME                         
        IS 'user-defined type name of the user-defined type of the data type being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.SCOPE_CATALOG                    
        IS 'catalog name of the referenceable table if DATA_TYPE is REF';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.SCOPE_SCHEMA                     
        IS 'schema name of the referenceable table if DATA_TYPE is REF';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.SCOPE_NAME                       
        IS 'scope name of the referenceable table if DATA_TYPE is REF';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.MAXIMUM_CARDINALITY              
        IS 'maximum cardinality if DATA_TYPE is ARRAY';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.DTD_IDENTIFIER                   
        IS 'data type descriptor identifier';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IS_SELF_REFERENCING              
        IS 'is a self-referencing column';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IS_IDENTITY                      
        IS 'is an identity column';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IDENTITY_GENERATION              
        IS 'for an identity column, the value is in ( ALWAYS, BY DEFAULT )';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IDENTITY_START                   
        IS 'for an identity column, the start value of the identity column';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IDENTITY_INCREMENT               
        IS 'for an identity column, the increment of the identity column';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IDENTITY_MAXIMUM                 
        IS 'for an identity column, the maximum value of the identity column';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IDENTITY_MINIMUM                 
        IS 'for an identity column, the minimum value of the identity column'; 
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IDENTITY_CYCLE                   
        IS 'for an identity column, the cycle option';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IS_GENERATED                     
        IS 'is a generated column';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.GENERATION_EXPRESSION            
        IS 'for a generated column, the text of the generation expression';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IS_SYSTEM_VERSION_START          
        IS 'is a system-version start column';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IS_SYSTEM_VERSION_END            
        IS 'is a system-version end column';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.SYSTEM_VERSION_TIMESTAMP_GENERATION  
        IS 'for a system-version column, the value is ALWAYS';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.IS_UPDATABLE                     
        IS 'is an updatable column';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.DECLARED_DATA_TYPE               
        IS 'the data type name that a user declared';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.DECLARED_NUMERIC_PRECISION       
        IS 'the precision value that a user declared'; 
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.DECLARED_NUMERIC_SCALE           
        IS 'the scale value that a user declared';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMNS.COMMENTS                         
        IS 'comments of the column';


--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.COLUMNS TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.COLUMN_PRIVILEGES
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.COLUMN_PRIVILEGES;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.COLUMN_PRIVILEGES
( 
       GRANTOR
     , GRANTEE
     , TABLE_CATALOG
     , TABLE_OWNER 
     , TABLE_SCHEMA 
     , TABLE_NAME 
     , COLUMN_NAME 
     , PRIVILEGE_TYPE
     , IS_GRANTABLE
)
AS
SELECT
       grantor.AUTHORIZATION_NAME
     , grantee.AUTHORIZATION_NAME
     , CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , owner.AUTHORIZATION_NAME
     , sch.SCHEMA_NAME
     , tab.TABLE_NAME
     , col.COLUMN_NAME
     , pvcol.PRIVILEGE_TYPE
     , pvcol.IS_GRANTABLE
  FROM
       DEFINITION_SCHEMA.COLUMN_PRIVILEGES AS pvcol 
     , DEFINITION_SCHEMA.COLUMNS           AS col 
     , DEFINITION_SCHEMA.TABLES            AS tab 
     , DEFINITION_SCHEMA.SCHEMATA          AS sch 
     , DEFINITION_SCHEMA.AUTHORIZATIONS    AS grantor
     , DEFINITION_SCHEMA.AUTHORIZATIONS    AS grantee
     , DEFINITION_SCHEMA.AUTHORIZATIONS    AS owner
 WHERE
       pvcol.COLUMN_ID  = col.COLUMN_ID
   AND pvcol.TABLE_ID   = tab.TABLE_ID
   AND pvcol.SCHEMA_ID  = sch.SCHEMA_ID
   AND pvcol.GRANTOR_ID = grantor.AUTH_ID
   AND pvcol.GRANTEE_ID = grantee.AUTH_ID
   AND tab.OWNER_ID     = owner.AUTH_ID
   AND ( grantee.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )
      -- OR  
      -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
      --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
         OR
         grantor.AUTHORIZATION_NAME = CURRENT_USER
       )
 ORDER BY 
       pvcol.SCHEMA_ID
     , pvcol.TABLE_ID
     , pvcol.COLUMN_ID
     , pvcol.GRANTOR_ID
     , pvcol.GRANTEE_ID
     , pvcol.PRIVILEGE_TYPE_ID   
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.COLUMN_PRIVILEGES
        IS 'Identify the privileges on columns of tables defined in this catalog that are available to or granted by a given user or role.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMN_PRIVILEGES.GRANTOR
        IS 'authorization name of the user who granted column privileges';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMN_PRIVILEGES.GRANTEE
        IS 'authorization name of some user or role, or PUBLIC to indicate all users, to whom the column privilege being described is granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMN_PRIVILEGES.TABLE_CATALOG
        IS 'catalog name of the column on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMN_PRIVILEGES.TABLE_OWNER 
        IS 'table owner name of the column on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMN_PRIVILEGES.TABLE_SCHEMA 
        IS 'schema name of the column on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMN_PRIVILEGES.TABLE_NAME 
        IS 'table name of the column on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMN_PRIVILEGES.COLUMN_NAME 
        IS 'column name of the column on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMN_PRIVILEGES.PRIVILEGE_TYPE
        IS 'the value is in ( SELECT, INSERT, UPDATE, REFERENCES )';
COMMENT ON COLUMN INFORMATION_SCHEMA.COLUMN_PRIVILEGES.IS_GRANTABLE
        IS 'is grantable';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.COLUMN_PRIVILEGES TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE
(
       TABLE_CATALOG
     , TABLE_OWNER
     , TABLE_SCHEMA
     , TABLE_NAME
     , COLUMN_NAME
     , CONSTRAINT_CATALOG
     , CONSTRAINT_OWNER
     , CONSTRAINT_SCHEMA
     , CONSTRAINT_NAME    
)
AS
SELECT
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth1.AUTHORIZATION_NAME
     , sch1.SCHEMA_NAME
     , tab.TABLE_NAME
     , col.COLUMN_NAME
     , CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth2.AUTHORIZATION_NAME
     , sch2.SCHEMA_NAME
     , tcon.CONSTRAINT_NAME
  FROM
       ( ( 
           SELECT 
                  CONSTRAINT_OWNER_ID
                , CONSTRAINT_SCHEMA_ID
                , CONSTRAINT_ID
                , TABLE_OWNER_ID
                , TABLE_SCHEMA_ID
                , TABLE_ID
                , COLUMN_ID
             FROM DEFINITION_SCHEMA.CHECK_COLUMN_USAGE ccu 
         )
         UNION ALL
         (
           SELECT
                  rfc.CONSTRAINT_OWNER_ID
                , rfc.CONSTRAINT_SCHEMA_ID
                , rfc.CONSTRAINT_ID
                , kcu.TABLE_OWNER_ID
                , kcu.TABLE_SCHEMA_ID
                , kcu.TABLE_ID
                , kcu.COLUMN_ID
             FROM DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS AS rfc
                , DEFINITION_SCHEMA.KEY_COLUMN_USAGE        AS kcu 
            WHERE
                  rfc.CONSTRAINT_ID = kcu.CONSTRAINT_ID
         ) 
         UNION ALL
         (
           SELECT
                  tcn.CONSTRAINT_OWNER_ID
                , tcn.CONSTRAINT_SCHEMA_ID
                , tcn.CONSTRAINT_ID
                , kcu.TABLE_OWNER_ID
                , kcu.TABLE_SCHEMA_ID
                , kcu.TABLE_ID
                , kcu.COLUMN_ID
             FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS       AS tcn
                , DEFINITION_SCHEMA.KEY_COLUMN_USAGE        AS kcu 
            WHERE
                  tcn.CONSTRAINT_ID = kcu.CONSTRAINT_ID
              AND tcn.CONSTRAINT_TYPE IN ( 'UNIQUE', 'PRIMARY KEY' )
         ) 
       ) AS vwccu ( CONSTRAINT_OWNER_ID
                  , CONSTRAINT_SCHEMA_ID
                  , CONSTRAINT_ID
                  , TABLE_OWNER_ID
                  , TABLE_SCHEMA_ID
                  , TABLE_ID
                  , COLUMN_ID )
     , DEFINITION_SCHEMA.COLUMNS            AS col
     , DEFINITION_SCHEMA.TABLES             AS tab
     , DEFINITION_SCHEMA.TABLE_CONSTRAINTS  AS tcon
     , DEFINITION_SCHEMA.SCHEMATA           AS sch1
     , DEFINITION_SCHEMA.SCHEMATA           AS sch2
     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS auth1
     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS auth2
 WHERE
       vwccu.COLUMN_ID            = col.COLUMN_ID
   AND vwccu.TABLE_ID             = tab.TABLE_ID
   AND vwccu.TABLE_SCHEMA_ID      = sch1.SCHEMA_ID
   AND vwccu.TABLE_OWNER_ID       = auth1.AUTH_ID
   AND vwccu.CONSTRAINT_ID        = tcon.CONSTRAINT_ID
   AND vwccu.CONSTRAINT_SCHEMA_ID = sch2.SCHEMA_ID
   AND vwccu.CONSTRAINT_OWNER_ID  = auth2.AUTH_ID
   AND ( 
         tab.OWNER_ID = USER_ID() 
         OR
         sch1.OWNER_ID = USER_ID()
         OR
         tcon.CONSTRAINT_OWNER_ID = USER_ID()
         OR
         sch2.OWNER_ID = USER_ID()
       )
 ORDER BY
       vwccu.TABLE_SCHEMA_ID
     , vwccu.TABLE_ID
     , vwccu.COLUMN_ID
     , vwccu.CONSTRAINT_ID
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE
        IS 'Identify the columns used by referential constraints, unique constraints, check constraints, and assertions defined in this catalog and owned by a given user or role.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE.TABLE_CATALOG
        IS 'catalog name of the column that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE.TABLE_OWNER
        IS 'owner name of the column that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE.TABLE_SCHEMA
        IS 'schema name of the column that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE.TABLE_NAME
        IS 'table name of the column that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE.COLUMN_NAME
        IS 'column name that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE.CONSTRAINT_CATALOG
        IS 'catalog name of the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE.CONSTRAINT_OWNER
        IS 'owner name of the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE.CONSTRAINT_SCHEMA
        IS 'schema name of the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE.CONSTRAINT_NAME
        IS 'constraint name';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE
(
       TABLE_CATALOG
     , TABLE_OWNER
     , TABLE_SCHEMA
     , TABLE_NAME
     , CONSTRAINT_CATALOG
     , CONSTRAINT_OWNER
     , CONSTRAINT_SCHEMA
     , CONSTRAINT_NAME    
)
AS
SELECT
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth1.AUTHORIZATION_NAME
     , sch1.SCHEMA_NAME
     , tab.TABLE_NAME
     , CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth2.AUTHORIZATION_NAME
     , sch2.SCHEMA_NAME
     , tcon.CONSTRAINT_NAME
  FROM
       ( ( 
           SELECT 
                  CONSTRAINT_OWNER_ID
                , CONSTRAINT_SCHEMA_ID
                , CONSTRAINT_ID
                , TABLE_OWNER_ID
                , TABLE_SCHEMA_ID
                , TABLE_ID
             FROM DEFINITION_SCHEMA.CHECK_TABLE_USAGE ctu 
         )
         UNION ALL
         (
           SELECT
                  rfc.CONSTRAINT_OWNER_ID
                , rfc.CONSTRAINT_SCHEMA_ID
                , rfc.CONSTRAINT_ID
                , tcn.TABLE_OWNER_ID
                , tcn.TABLE_SCHEMA_ID
                , tcn.TABLE_ID
             FROM DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS AS rfc
                , DEFINITION_SCHEMA.TABLE_CONSTRAINTS       AS tcn
            WHERE
                  rfc.CONSTRAINT_ID = tcn.CONSTRAINT_ID
         ) 
         UNION ALL
         (
           SELECT
                  tcn.CONSTRAINT_OWNER_ID
                , tcn.CONSTRAINT_SCHEMA_ID
                , tcn.CONSTRAINT_ID
                , tcn.TABLE_OWNER_ID
                , tcn.TABLE_SCHEMA_ID
                , tcn.TABLE_ID
             FROM DEFINITION_SCHEMA.TABLE_CONSTRAINTS       AS tcn
            WHERE
                  tcn.CONSTRAINT_TYPE IN ( 'UNIQUE', 'PRIMARY KEY' )
         ) 
       ) AS vwctu ( CONSTRAINT_OWNER_ID
                  , CONSTRAINT_SCHEMA_ID
                  , CONSTRAINT_ID
                  , TABLE_OWNER_ID
                  , TABLE_SCHEMA_ID
                  , TABLE_ID )
     , DEFINITION_SCHEMA.TABLES             AS tab
     , DEFINITION_SCHEMA.TABLE_CONSTRAINTS  AS tcon
     , DEFINITION_SCHEMA.SCHEMATA           AS sch1
     , DEFINITION_SCHEMA.SCHEMATA           AS sch2
     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS auth1
     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS auth2
 WHERE
       vwctu.TABLE_ID             = tab.TABLE_ID
   AND vwctu.TABLE_SCHEMA_ID      = sch1.SCHEMA_ID
   AND vwctu.TABLE_OWNER_ID       = auth1.AUTH_ID
   AND vwctu.CONSTRAINT_ID        = tcon.CONSTRAINT_ID
   AND vwctu.CONSTRAINT_SCHEMA_ID = sch2.SCHEMA_ID
   AND vwctu.CONSTRAINT_OWNER_ID  = auth2.AUTH_ID
   AND ( 
         tab.OWNER_ID = USER_ID() 
         OR
         sch1.OWNER_ID = USER_ID()
         OR
         tcon.CONSTRAINT_OWNER_ID = USER_ID()
         OR
         sch2.OWNER_ID = USER_ID()
       )
 ORDER BY
       vwctu.TABLE_SCHEMA_ID
     , vwctu.TABLE_ID
     , vwctu.CONSTRAINT_ID
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE
        IS 'Identify the tables that are used by referential constraints, unique constraints, check constraints, and assertions defined in this catalog and owned by a given user or role.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE.TABLE_CATALOG
        IS 'catalog name of the table that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE.TABLE_OWNER
        IS 'owner name of the table that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE.TABLE_SCHEMA
        IS 'schema name of the table that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE.TABLE_NAME
        IS 'table name that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE.CONSTRAINT_CATALOG
        IS 'catalog name of the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE.CONSTRAINT_OWNER
        IS 'owner name of the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE.CONSTRAINT_SCHEMA
        IS 'schema name of the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE.CONSTRAINT_NAME
        IS 'constraint name';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.CONSTRAINT_TABLE_USAGE TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.INFORMATION_SCHEMA_CATALOG_NAME
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.INFORMATION_SCHEMA_CATALOG_NAME;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.INFORMATION_SCHEMA_CATALOG_NAME
( 
       CATALOG_NAME
)
AS 
SELECT
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
  FROM DUAL
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.INFORMATION_SCHEMA_CATALOG_NAME
        IS 'Identify the catalog that contains the Information Schema';
          
--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.INFORMATION_SCHEMA_CATALOG_NAME.CATALOG_NAME
        IS 'the name of catalog in which this Information Schema resides';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.INFORMATION_SCHEMA_CATALOG_NAME TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.KEY_COLUMN_USAGE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.KEY_COLUMN_USAGE;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.KEY_COLUMN_USAGE
(
       CONSTRAINT_CATALOG
     , CONSTRAINT_OWNER
     , CONSTRAINT_SCHEMA
     , CONSTRAINT_NAME    
     , TABLE_CATALOG
     , TABLE_OWNER
     , TABLE_SCHEMA
     , TABLE_NAME
     , COLUMN_NAME
     , ORDINAL_POSITION
     , POSITION_IN_UNIQUE_CONSTRAINT
)
AS
SELECT
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) )      -- CONSTRAINT_CATALOG
     , auth1.AUTHORIZATION_NAME                            -- CONSTRAINT_OWNER
     , sch1.SCHEMA_NAME                                    -- CONSTRAINT_SCHEMA
     , tcn.CONSTRAINT_NAME                                 -- CONSTRAINT_NAME    
     , CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) )      -- TABLE_CATALOG
     , auth2.AUTHORIZATION_NAME                            -- TABLE_OWNER
     , sch2.SCHEMA_NAME                                    -- TABLE_SCHEMA
     , tab.TABLE_NAME                                      -- TABLE_NAME
     , col.COLUMN_NAME                                     -- COLUMN_NAME
     , CAST( kcu.ORDINAL_POSITION AS NUMBER )              -- ORDINAL_POSITION
     , CAST( kcu.POSITION_IN_UNIQUE_CONSTRAINT AS NUMBER ) -- POSITION_IN_UNIQUE_CONSTRAINT
  FROM
       DEFINITION_SCHEMA.KEY_COLUMN_USAGE   AS kcu 
     , DEFINITION_SCHEMA.TABLE_CONSTRAINTS  AS tcn
     , DEFINITION_SCHEMA.COLUMNS            AS col
     , DEFINITION_SCHEMA.TABLES             AS tab
     , DEFINITION_SCHEMA.SCHEMATA           AS sch1
     , DEFINITION_SCHEMA.SCHEMATA           AS sch2
     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS auth1
     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS auth2
 WHERE
       kcu.CONSTRAINT_ID        = tcn.CONSTRAINT_ID
   AND kcu.CONSTRAINT_SCHEMA_ID = sch1.SCHEMA_ID
   AND kcu.CONSTRAINT_OWNER_ID  = auth1.AUTH_ID
   AND kcu.COLUMN_ID            = col.COLUMN_ID
   AND kcu.TABLE_ID             = tab.TABLE_ID
   AND kcu.TABLE_SCHEMA_ID      = sch2.SCHEMA_ID
   AND kcu.TABLE_OWNER_ID       = auth2.AUTH_ID
   AND ( 
         ( SELECT MAX( kcu2.ORDINAL_POSITION )
             FROM DEFINITION_SCHEMA.KEY_COLUMN_USAGE   AS kcu2
            WHERE kcu2.CONSTRAINT_ID = kcu.CONSTRAINT_ID )
         =
         ( SELECT COUNT(*)
             FROM DEFINITION_SCHEMA.KEY_COLUMN_USAGE   AS kcu3
            WHERE kcu3.CONSTRAINT_ID = kcu.CONSTRAINT_ID
              AND kcu3.COLUMN_ID IN ( SELECT pvcol.COLUMN_ID
                                        FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES AS pvcol
                                       WHERE pvcol.PRIVILEGE_TYPE <> 'SELECT'
                                         AND pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                                    FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS aucol 
                                                                   WHERE aucol.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                                 ) 
                                       -- OR  
                                       -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                       --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                    )
         )
         OR 
         tcn.TABLE_ID IN ( SELECT pvtab.TABLE_ID 
                             FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES AS pvtab 
                            WHERE pvtab.PRIVILEGE_TYPE <> 'SELECT'
                              AND ( pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS autab 
                                                           WHERE autab.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                        ) 
                                 -- OR  
                                 -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                 --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                  )
                          ) 
         OR 
         tcn.TABLE_SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID 
                                    FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch 
                                   WHERE pvsch.PRIVILEGE_TYPE IN ( 'CONTROL SCHEMA', 'INSERT TABLE', 'DELETE TABLE', 'UPDATE TABLE' ) 
                                     AND ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                                     FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch 
                                                                    WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                                 ) 
                                          -- OR  
                                          -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                          --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                           )  
                                   ) 
         OR 
         EXISTS ( SELECT GRANTEE_ID  
                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba 
                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'INSERT ANY TABLE', 'DELETE ANY TABLE', 'UPDATE ANY TABLE' ) 
                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba 
                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                               ) 
                        -- OR  
                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                          )  
                ) 
       ) 
 ORDER BY
       kcu.TABLE_SCHEMA_ID
     , kcu.TABLE_ID
     , kcu.CONSTRAINT_ID
     , kcu.ORDINAL_POSITION
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.KEY_COLUMN_USAGE
        IS 'Identify the columns defined in this catalog that are constrained as keys and that are accessible by a given user or role.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.KEY_COLUMN_USAGE.CONSTRAINT_CATALOG
        IS 'catalog name of the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.KEY_COLUMN_USAGE.CONSTRAINT_OWNER
        IS 'owner name of the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.KEY_COLUMN_USAGE.CONSTRAINT_SCHEMA
        IS 'schema name of the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.KEY_COLUMN_USAGE.CONSTRAINT_NAME
        IS 'constraint name';
COMMENT ON COLUMN INFORMATION_SCHEMA.KEY_COLUMN_USAGE.TABLE_CATALOG
        IS 'catalog name of the column that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.KEY_COLUMN_USAGE.TABLE_OWNER
        IS 'owner name of the column that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.KEY_COLUMN_USAGE.TABLE_SCHEMA
        IS 'schema name of the column that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.KEY_COLUMN_USAGE.TABLE_NAME
        IS 'table name of the column that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.KEY_COLUMN_USAGE.COLUMN_NAME
        IS 'column name that participates in the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.KEY_COLUMN_USAGE.ORDINAL_POSITION
        IS 'the ordinal position of the specific column in the constraint being described. If the constraint described is a key of cardinality 1 (one), then the value of ORDINAL_POSITION is always 1 (one).';
COMMENT ON COLUMN INFORMATION_SCHEMA.KEY_COLUMN_USAGE.POSITION_IN_UNIQUE_CONSTRAINT
        IS 'If the constraint being described is a foreign key constraint, then the value of POSITION_IN_UNIQUE_CONSTRAINT is the ordinal position of the referenced column corresponding to the referencing column being described, in the corresponding unique key constraint.';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.KEY_COLUMN_USAGE TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS 
(
       CONSTRAINT_CATALOG
     , CONSTRAINT_OWNER
     , CONSTRAINT_SCHEMA
     , CONSTRAINT_NAME
     , CONSTRAINT_TABLE_NAME
     , CONSTRAINT_COLUMN_NAME
     , ORDINAL_POSITION
     , UNIQUE_CONSTRAINT_CATALOG
     , UNIQUE_CONSTRAINT_OWNER
     , UNIQUE_CONSTRAINT_SCHEMA
     , UNIQUE_CONSTRAINT_NAME
     , UNIQUE_CONSTRAINT_TABLE_NAME
     , UNIQUE_CONSTRAINT_COLUMN_NAME
     , IS_PRIMARY_KEY
     , MATCH_OPTION
     , UPDATE_RULE
     , DELETE_RULE
     , IS_DEFERRABLE
     , INITIALLY_DEFERRED
)
AS
SELECT 
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth1.AUTHORIZATION_NAME
     , sch1.SCHEMA_NAME
     , tcon.CONSTRAINT_NAME
     , rtab.TABLE_NAME
     , rcol.COLUMN_NAME
     , CAST( rkcu.ORDINAL_POSITION AS NUMBER )
     , CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth2.AUTHORIZATION_NAME
     , sch2.SCHEMA_NAME
     , ucon.CONSTRAINT_NAME
     , utab.TABLE_NAME
     , ucol.COLUMN_NAME
     , CAST( CASE WHEN ucon.CONSTRAINT_TYPE = 'PRIMARY KEY' 
                  THEN TRUE
                  ELSE FALSE
                  END
             AS BOOLEAN ) 
     , rcon.MATCH_OPTION
     , rcon.UPDATE_RULE
     , rcon.DELETE_RULE
     , tcon.IS_DEFERRABLE
     , tcon.INITIALLY_DEFERRED
  FROM 
       DEFINITION_SCHEMA.REFERENTIAL_CONSTRAINTS  AS rcon
     , DEFINITION_SCHEMA.TABLE_CONSTRAINTS        AS tcon
     , DEFINITION_SCHEMA.KEY_COLUMN_USAGE         AS rkcu
     , DEFINITION_SCHEMA.TABLE_CONSTRAINTS        AS ucon
     , DEFINITION_SCHEMA.KEY_COLUMN_USAGE         AS ukcu
     , DEFINITION_SCHEMA.COLUMNS                  AS rcol
     , DEFINITION_SCHEMA.COLUMNS                  AS ucol
     , DEFINITION_SCHEMA.TABLES                   AS rtab
     , DEFINITION_SCHEMA.TABLES                   AS utab
     , DEFINITION_SCHEMA.SCHEMATA                 AS sch1 
     , DEFINITION_SCHEMA.SCHEMATA                 AS sch2 
     , DEFINITION_SCHEMA.AUTHORIZATIONS           AS auth1 
     , DEFINITION_SCHEMA.AUTHORIZATIONS           AS auth2 
 WHERE 
       rcon.CONSTRAINT_ID                = tcon.CONSTRAINT_ID
   AND rcon.UNIQUE_CONSTRAINT_ID         = ucon.CONSTRAINT_ID
   AND tcon.CONSTRAINT_ID                = rkcu.CONSTRAINT_ID
   AND tcon.TABLE_ID                     = rtab.TABLE_ID
   AND rkcu.COLUMN_ID                    = rcol.COLUMN_ID
   AND ucon.CONSTRAINT_ID                = ukcu.CONSTRAINT_ID
   AND ucon.TABLE_ID                     = utab.TABLE_ID
   AND rkcu.POSITION_IN_UNIQUE_CONSTRAINT = ukcu.ORDINAL_POSITION
   AND ukcu.COLUMN_ID                    = ucol.COLUMN_ID
   AND rcon.CONSTRAINT_SCHEMA_ID         = sch1.SCHEMA_ID
   AND rcon.CONSTRAINT_OWNER_ID          = auth1.AUTH_ID
   AND rcon.UNIQUE_CONSTRAINT_SCHEMA_ID  = sch2.SCHEMA_ID
   AND rcon.UNIQUE_CONSTRAINT_OWNER_ID   = auth2.AUTH_ID
   AND ( tcon.TABLE_ID IN ( SELECT pvcol.TABLE_ID 
                              FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES AS pvcol 
                             WHERE pvcol.PRIVILEGE_TYPE <> 'SELECT'
                               AND ( pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS aucol 
                                                            WHERE aucol.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                         ) 
                                    -- OR  
                                    -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                    --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                   )
                          ) 
         OR 
         tcon.TABLE_ID IN ( SELECT pvtab.TABLE_ID 
                              FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES AS pvtab 
                             WHERE pvtab.PRIVILEGE_TYPE <> 'SELECT'
                               AND ( pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS autab 
                                                            WHERE autab.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                         ) 
                                  -- OR  
                                  -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                   )
                         ) 
         OR 
         tcon.TABLE_SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID 
                                     FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch 
                                    WHERE pvsch.PRIVILEGE_TYPE IN ( 'CONTROL SCHEMA', 'INSERT TABLE', 'DELETE TABLE', 'UPDATE TABLE' ) 
                                      AND ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                                    FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch 
                                                                   WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                                ) 
                                         -- OR  
                                         -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                         --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                          )  
                                  ) 
         OR 
         EXISTS ( SELECT GRANTEE_ID  
                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba 
                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'INSERT ANY TABLE', 'DELETE ANY TABLE', 'UPDATE ANY TABLE' ) 
                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba 
                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                ) 
                        -- OR  
                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                          )  
                ) 
       ) 
ORDER BY 
      rcon.CONSTRAINT_SCHEMA_ID 
    , rcon.CONSTRAINT_ID
    , rkcu.ORDINAL_POSITION
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS 
        IS 'Identify the referential constraints defined on tables in this catalog that are accssible to a given user or role.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.CONSTRAINT_CATALOG
        IS 'catalog name of the referential constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.CONSTRAINT_OWNER
        IS 'owner name who owns the referential constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.CONSTRAINT_SCHEMA
        IS 'schema name of the referential constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.CONSTRAINT_NAME
        IS 'referential constraint name';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.CONSTRAINT_TABLE_NAME
        IS 'name of the table to which the referential constraint being described applies';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.CONSTRAINT_COLUMN_NAME
        IS 'column name of the table to which the referential constraint being described applies';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.ORDINAL_POSITION
        IS 'the ordinal position of the specific column in the referentail constraint being described.';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.UNIQUE_CONSTRAINT_CATALOG
        IS 'catalog name of the unique or primary key constraint applied to the referenced column list being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.UNIQUE_CONSTRAINT_OWNER
        IS 'owner name of the unique or primary key constraint applied to the referenced column list being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.UNIQUE_CONSTRAINT_SCHEMA
        IS 'schema name of the unique or primary key constraint applied to the referenced column list being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.UNIQUE_CONSTRAINT_NAME
        IS 'constraint name of the unique or primary key constraint applied to the referenced column list being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.UNIQUE_CONSTRAINT_TABLE_NAME
        IS 'table name of the unique or primary key constraint applied to the referenced column list being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.UNIQUE_CONSTRAINT_COLUMN_NAME
        IS 'column name of the unique or primary key constraint applied to the referenced column list being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.IS_PRIMARY_KEY
        IS 'whether the constraint applied to the referenced column list being described, is primary key or not';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.MATCH_OPTION
        IS 'the referential constraint that has a match option: the value in ( SIMPLE, PARTIAL, FULL )';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.UPDATE_RULE
        IS 'the referential constraint that has an update rule: the value in ( NO ACTION, RESTRICT, CASCADE, SET NULL, SET DEFAULT )';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.DELETE_RULE
        IS 'the referential constraint that has a delete rule: the value in ( NO ACTION, RESTRICT, CASCADE, SET NULL, SET DEFAULT )';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.IS_DEFERRABLE
        IS 'is a deferrable constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS.INITIALLY_DEFERRED
        IS 'is an initially deferred constraint';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.SCHEMATA
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.SCHEMATA;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.SCHEMATA 
(
       CATALOG_NAME
     , SCHEMA_NAME
     , SCHEMA_OWNER
     , DEFAULT_CHARACTER_SET_CATALOG
     , DEFAULT_CHARACTER_SET_SCHEMA
     , DEFAULT_CHARACTER_SET_NAME
     , SQL_PATH
     , CREATED_TIME
     , MODIFIED_TIME
     , COMMENTS
)
AS
SELECT 
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , sch.SCHEMA_NAME
     , auth.AUTHORIZATION_NAME
     , sch.DEFAULT_CHARACTER_SET_CATALOG
     , sch.DEFAULT_CHARACTER_SET_SCHEMA
     , sch.DEFAULT_CHARACTER_SET_NAME
     , sch.SQL_PATH
     , sch.CREATED_TIME
     , sch.MODIFIED_TIME
     , sch.COMMENTS
  FROM 
       DEFINITION_SCHEMA.SCHEMATA        AS sch
     , DEFINITION_SCHEMA.AUTHORIZATIONS  AS auth
 WHERE 
       sch.OWNER_ID = auth.AUTH_ID
   AND ( sch.OWNER_ID = USER_ID()
         OR
         sch.SCHEMA_ID IN ( SELECT pvcol.SCHEMA_ID 
                              FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES AS pvcol 
                             WHERE ( pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS aucol 
                                                            WHERE aucol.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                         ) 
                                  -- OR  
                                  -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                   )
                          ) 
         OR 
         sch.SCHEMA_ID IN ( SELECT pvtab.SCHEMA_ID 
                             FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES AS pvtab 
                            WHERE ( pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS autab 
                                                           WHERE autab.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                        ) 
                                 -- OR  
                                 -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                 --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                  )  
                          ) 
         OR
         sch.SCHEMA_ID IN ( SELECT pvusg.SCHEMA_ID 
                             FROM DEFINITION_SCHEMA.USAGE_PRIVILEGES AS pvusg
                            WHERE ( pvusg.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS auusg 
                                                           WHERE auusg.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                        ) 
                                 -- OR  
                                 -- pvusg.GRANTEE_ID IN ( SELECT AUTH_ID 
                                 --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                  )  
                          ) 
         OR
         sch.SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID 
                              FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch 
                                     WHERE ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                                     FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch 
                                                                    WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                                 ) 
                                          -- OR  
                                          -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                          --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                           )  
                                   ) 
         OR 
         EXISTS ( SELECT GRANTEE_ID  
                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba 
                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'ALTER SCHEMA', 'DROP SCHEMA', 
                                                   'CREATE ANY TABLE', 'ALTER ANY TABLE', 'DROP ANY TABLE', 
                                                   'SELECT ANY TABLE', 'INSERT ANY TABLE', 'DELETE ANY TABLE', 'UPDATE ANY TABLE', 'LOCK ANY TABLE',
                                                   'CREATE ANY VIEW', 'DROP ANY VIEW', 
                                                   'CREATE ANY SEQUENCE', 'ALTER ANY SEQUENCE', 'DROP ANY SEQUENCE', 'USAGE ANY SEQUENCE',
                                                   'CREATE ANY INDEX', 'ALTER ANY INDEX', 'DROP ANY INDEX' ) 
                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba 
                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                               ) 
                        -- OR  
                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                          )  
                ) 
       ) 
 ORDER BY
       sch.SCHEMA_ID
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.SCHEMATA 
        IS 'Identify the schemata in a catalog that are owned by given user or accessible to given user or role.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.SCHEMATA.CATALOG_NAME
        IS 'catalog name of the schema';
COMMENT ON COLUMN INFORMATION_SCHEMA.SCHEMATA.SCHEMA_NAME
        IS 'schema name';
COMMENT ON COLUMN INFORMATION_SCHEMA.SCHEMATA.SCHEMA_OWNER
        IS 'authorization name who owns the schema';
COMMENT ON COLUMN INFORMATION_SCHEMA.SCHEMATA.DEFAULT_CHARACTER_SET_CATALOG
        IS 'catalog name of the default character set for columns and domains in the schemata';
COMMENT ON COLUMN INFORMATION_SCHEMA.SCHEMATA.DEFAULT_CHARACTER_SET_SCHEMA
        IS 'schema name of the default character set for columns and domains in the schemata';
COMMENT ON COLUMN INFORMATION_SCHEMA.SCHEMATA.DEFAULT_CHARACTER_SET_NAME
        IS 'character set name of the default character set for columns and domains in the schemata';
COMMENT ON COLUMN INFORMATION_SCHEMA.SCHEMATA.SQL_PATH
        IS 'character representation of schema path specification';
COMMENT ON COLUMN INFORMATION_SCHEMA.SCHEMATA.CREATED_TIME
        IS 'created time of the schema';
COMMENT ON COLUMN INFORMATION_SCHEMA.SCHEMATA.MODIFIED_TIME
        IS 'last modified time of the schema';
COMMENT ON COLUMN INFORMATION_SCHEMA.SCHEMATA.COMMENTS
        IS 'comments of the schema';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.SCHEMATA TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.SEQUENCES
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.SEQUENCES;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.SEQUENCES 
( 
       SEQUENCE_CATALOG
     , SEQUENCE_OWNER
     , SEQUENCE_SCHEMA
     , SEQUENCE_NAME
     , DATA_TYPE
     , NUMERIC_PRECISION
     , NUMERIC_PRECISION_RADIX
     , NUMERIC_SCALE
     , START_VALUE
     , MINIMUM_VALUE
     , MAXIMUM_VALUE
     , INCREMENT
     , CYCLE_OPTION
     , CACHE_SIZE
     , DECLARED_DATA_TYPE
     , DECLARED_NUMERIC_PRECISION
     , DECLARED_NUMERIC_SCALE
     , CREATED_TIME
     , MODIFIED_TIME
     , COMMENTS
)
AS
SELECT 
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth.AUTHORIZATION_NAME
     , sch.SCHEMA_NAME
     , sqc.SEQUENCE_NAME
     , CAST( 'NATIVE_BIGINT' AS VARCHAR(128 OCTETS) )
     , CAST( 63 AS NUMBER )
     , CAST( 2 AS NUMBER )
     , CAST( 0 AS NUMBER )
     , CAST( sqc.START_VALUE AS NUMBER )
     , CAST( sqc.MINIMUM_VALUE AS NUMBER )
     , CAST( sqc.MAXIMUM_VALUE AS NUMBER )
     , CAST( sqc.INCREMENT AS NUMBER )
     , sqc.CYCLE_OPTION
     , sqc.CACHE_SIZE
     , CAST( NULL AS VARCHAR(128 OCTETS) )
     , CAST( NULL AS NUMBER )
     , CAST( NULL AS NUMBER )
     , sqc.CREATED_TIME
     , sqc.MODIFIED_TIME
     , sqc.COMMENTS
  FROM 
       DEFINITION_SCHEMA.SEQUENCES       AS sqc
     , DEFINITION_SCHEMA.SCHEMATA        AS sch
     , DEFINITION_SCHEMA.AUTHORIZATIONS  AS auth
 WHERE 
       sqc.SCHEMA_ID = sch.SCHEMA_ID
   AND sqc.OWNER_ID  = auth.AUTH_ID
   AND ( sqc.SEQUENCE_ID IN ( SELECT pvusg.OBJECT_ID 
                                FROM DEFINITION_SCHEMA.USAGE_PRIVILEGES AS pvusg
                               WHERE pvusg.OBJECT_TYPE = 'SEQUENCE'
                                 AND ( pvusg.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                               FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS auusg 
                                                              WHERE auusg.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                           ) 
                                    -- OR  
                                    -- pvusg.GRANTEE_ID IN ( SELECT AUTH_ID 
                                    --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                     )  
                            ) 
         OR
         sqc.SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID 
                              FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch 
                             WHERE pvsch.PRIVILEGE_TYPE IN ( 'CONTROL SCHEMA', 'ALTER SEQUENCE', 'DROP SEQUENCE', 'USAGE SEQUENCE' )
                               AND ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch 
                                                            WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                          ) 
                                  -- OR  
                                  -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                   )  
                           ) 
         OR 
         EXISTS ( SELECT GRANTEE_ID  
                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba 
                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'ALTER ANY SEQUENCE', 'DROP ANY SEQUENCE', 'USAGE ANY SEQUENCE' ) 
                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba 
                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                               ) 
                        -- OR  
                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                          )  
                ) 
       ) 
 ORDER BY
       sqc.SCHEMA_ID
     , sqc.SEQUENCE_ID
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.SEQUENCES 
        IS 'Identify the external sequence generators defined in this catalog that are accesible to a given user or role.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.SEQUENCE_CATALOG
        IS 'catalog name of the sequence';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.SEQUENCE_OWNER
        IS 'owner name of the sequence';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.SEQUENCE_SCHEMA
        IS 'schema name of the sequence';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.SEQUENCE_NAME
        IS 'sequence name';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.DATA_TYPE
        IS 'the standard name of the data type';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.NUMERIC_PRECISION
        IS 'the numeric precision of the numerical data type';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.NUMERIC_PRECISION_RADIX
        IS 'the radix ( 2 or 10 ) of the precision of the numerical data type';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.NUMERIC_SCALE
        IS 'the numeric scale of the exact numerical data type';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.START_VALUE
        IS 'the start value of the sequence generator';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.MINIMUM_VALUE
        IS 'the minimum value of the sequence generator';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.MAXIMUM_VALUE
        IS 'the maximum value of the sequence generator';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.INCREMENT
        IS 'the increment of the sequence generator';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.CYCLE_OPTION
        IS 'cycle option';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.CACHE_SIZE
        IS 'number of sequence numbers to cache';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.DECLARED_DATA_TYPE
        IS 'the data type name that a user declared';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.DECLARED_NUMERIC_PRECISION
        IS 'the precision value that a user declared';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.DECLARED_NUMERIC_SCALE
        IS 'the scale value that a user declared';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.CREATED_TIME
        IS 'created time of the sequence generator';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.MODIFIED_TIME
        IS 'last modified time of the sequence generator';
COMMENT ON COLUMN INFORMATION_SCHEMA.SEQUENCES.COMMENTS
        IS 'comments of the sequence generator';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.SEQUENCES TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.SQL_FEATURES
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.SQL_FEATURES;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.SQL_FEATURES
( 
       FEATURE_ID
     , FEATURE_NAME
     , SUB_FEATURE_ID
     , SUB_FEATURE_NAME
     , IS_SUPPORTED
     , IS_VERIFIED_BY
     , COMMENTS
)
AS
SELECT
       ID
     , NAME
     , SUB_ID
     , SUB_NAME
     , IS_SUPPORTED
     , IS_VERIFIED_BY
     , COMMENTS
  FROM 
       DEFINITION_SCHEMA.SQL_CONFORMANCE
 WHERE
       TYPE IN ( 'FEATURE', 'SUBFEATURE' )
 ORDER BY
       ID
     , SUB_ID
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.SQL_FEATURES 
        IS 'List the features and subfeatures of this ISO/IEC 9075 standard, and indicate which of these the SQL-implementation supports.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_FEATURES.FEATURE_ID
        IS 'identifier string of the conformance element';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_FEATURES.FEATURE_NAME
        IS 'descriptive name of the conformance element';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_FEATURES.SUB_FEATURE_ID
        IS 'identifier string of the subfeature, or a single space if not a subfeature';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_FEATURES.SUB_FEATURE_NAME
        IS 'descriptive name of the subfeature, or a single space if not a subfeature';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_FEATURES.IS_SUPPORTED
        IS 'TRUE if an SQL-implementation fully supports that conformance element described when SQL-data in the identified catalog is accessed through that implementation, FALSE if not';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_FEATURES.IS_VERIFIED_BY
        IS 'If full support for the conformance element described has been verified by testing, then the IS_VERIFIED_BY column shall contain information identifying the conformance test used to verify the conformance claim; otherwise, IS_VERIFIED_BY shall be the null value';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_FEATURES.COMMENTS
        IS 'possibly a comment pertaining to the conformance element';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.SQL_FEATURES TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.SQL_IMPLEMENTATION_INFO
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.SQL_IMPLEMENTATION_INFO;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.SQL_IMPLEMENTATION_INFO
( 
       IMPLEMENTATION_INFO_ID
     , IMPLEMENTATION_INFO_NAME
     , INTEGER_VALUE
     , CHARACTER_VALUE
     , COMMENTS
)
AS
SELECT
       IMPLEMENTATION_INFO_ID
     , IMPLEMENTATION_INFO_NAME
     , CASE WHEN ( INTEGER_VALUE < 0 ) THEN NULL ELSE INTEGER_VALUE END
     , CHARACTER_VALUE
     , COMMENTS
  FROM 
       DEFINITION_SCHEMA.SQL_IMPLEMENTATION_INFO
 ORDER BY
       IMPLEMENTATION_INFO_ID
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.SQL_IMPLEMENTATION_INFO 
        IS 'List the SQL-implementation information items defined in this ISO/IEC 9075 standard and, for each of these, indicate the value supported by the SQL-implementation.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_IMPLEMENTATION_INFO.IMPLEMENTATION_INFO_ID
        IS 'identifier string of the implementation information item';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_IMPLEMENTATION_INFO.IMPLEMENTATION_INFO_NAME
        IS 'descriptive name of the implementation information item';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_IMPLEMENTATION_INFO.INTEGER_VALUE
        IS 'value of the implementation information item, or null if the value is contained in the column CHARACTER_VALUE';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_IMPLEMENTATION_INFO.CHARACTER_VALUE
        IS 'value of the implementation information item, or null if the value is contained in the column INTEGER_VALUE';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_IMPLEMENTATION_INFO.COMMENTS
        IS 'possibly a comment pertaining to the implementation information item';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.SQL_IMPLEMENTATION_INFO TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.SQL_PACKAGES
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.SQL_PACKAGES;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.SQL_PACKAGES
( 
       ID
     , NAME
     , IS_SUPPORTED
     , IS_VERIFIED_BY
     , COMMENTS
)
AS
SELECT
       ID
     , NAME
     , IS_SUPPORTED
     , IS_VERIFIED_BY
     , COMMENTS
  FROM 
       DEFINITION_SCHEMA.SQL_CONFORMANCE
 WHERE TYPE = 'PACKAGE'
 ORDER BY
       ID
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.SQL_PACKAGES 
        IS 'List the packages of this ISO/IEC 9075 standard, and indicate which of these the SQL-implementation supports.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_PACKAGES.ID
        IS 'identifier string of the conformance element';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_PACKAGES.NAME
        IS 'descriptive name of the conformance element';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_PACKAGES.IS_SUPPORTED
        IS 'TRUE if an SQL-implementation fully supports that conformance element described when SQL-data in the identified catalog is accessed through that implementation, FALSE if not';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_PACKAGES.IS_VERIFIED_BY
        IS 'If full support for the conformance element described has been verified by testing, then the IS_VERIFIED_BY column shall contain information identifying the conformance test used to verify the conformance claim; otherwise, IS_VERIFIED_BY shall be the null value';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_PACKAGES.COMMENTS
        IS 'possibly a comment pertaining to the conformance element';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.SQL_PACKAGES TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.SQL_PARTS
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.SQL_PARTS;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.SQL_PARTS
( 
       ID
     , NAME
     , IS_SUPPORTED
     , IS_VERIFIED_BY
     , COMMENTS
)
AS
SELECT
       ID
     , NAME
     , IS_SUPPORTED
     , IS_VERIFIED_BY
     , COMMENTS
  FROM 
       DEFINITION_SCHEMA.SQL_CONFORMANCE
 WHERE TYPE = 'PART'
 ORDER BY
       CAST( ID AS NUMBER )
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.SQL_PARTS 
        IS 'List the parts of this ISO/IEC 9075 standard, and indicate which of these the SQL-implementation supports.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_PARTS.ID
        IS 'identifier string of the conformance element';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_PARTS.NAME
        IS 'descriptive name of the conformance element';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_PARTS.IS_SUPPORTED
        IS 'TRUE if an SQL-implementation fully supports that conformance element described when SQL-data in the identified catalog is accessed through that implementation, FALSE if not';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_PARTS.IS_VERIFIED_BY
        IS 'If full support for the conformance element described has been verified by testing, then the IS_VERIFIED_BY column shall contain information identifying the conformance test used to verify the conformance claim; otherwise, IS_VERIFIED_BY shall be the null value';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_PARTS.COMMENTS
        IS 'possibly a comment pertaining to the conformance element';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.SQL_PARTS TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.SQL_SIZING
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.SQL_SIZING;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.SQL_SIZING
( 
       SIZING_ID
     , SIZING_NAME
     , SUPPORTED_VALUE
     , COMMENTS
)
AS
SELECT
       SIZING_ID
     , SIZING_NAME
     , SUPPORTED_VALUE
     , COMMENTS
  FROM 
       DEFINITION_SCHEMA.SQL_SIZING
 ORDER BY
       SIZING_ID
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.SQL_SIZING 
        IS 'List the sizing items of this ISO/IEC 9075 standard, for each of these, indicate the size supported by the SQL-implementation.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_SIZING.SIZING_ID
        IS 'identifier of the sizing item';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_SIZING.SIZING_NAME
        IS 'descriptive name of the sizing item';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_SIZING.SUPPORTED_VALUE
        IS 'value of the sizing item, or 0 if the size is unlimited or cannot be determined, or null if the features for which the sizing item is applicable are not supported';
COMMENT ON COLUMN INFORMATION_SCHEMA.SQL_SIZING.COMMENTS
        IS 'possibly a comment pertaining to the sizing item';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.SQL_SIZING TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.STATISTICS
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.STATISTICS;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.STATISTICS 
( 
       TABLE_CATALOG 
     , TABLE_OWNER 
     , TABLE_SCHEMA 
     , TABLE_NAME 
     , STAT_TYPE
     , NON_UNIQUE
     , INDEX_CATALOG
     , INDEX_OWNER
     , INDEX_SCHEMA
     , INDEX_NAME
     , COLUMN_NAME
     , ORDINAL_POSITION
     , IS_ASCENDING_ORDER
     , IS_NULLS_FIRST
     , CARDINALITY
     , PAGES
     , FILTER_CONDITION
     , COMMENTS 
) 
AS
(
SELECT
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) )  -- TABLE_CATALOG 
     , auth2.AUTHORIZATION_NAME                        -- TABLE_OWNER 
     , sch2.SCHEMA_NAME                                -- TABLE_SCHEMA 
     , tab.TABLE_NAME                                  -- TABLE_NAME 
     , CAST( CASE idx.INDEX_TYPE WHEN 'HASH' THEN 'INDEX HASHED'
                                 ELSE 'INDEX OTHER'
                                 END
             AS VARCHAR(32 OCTETS) )                   -- STAT_TYPE
     , NOT idx.IS_UNIQUE                               -- NON_UNIQUE
     , CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) )  -- INDEX_CATALOG
     , auth1.AUTHORIZATION_NAME                        -- INDEX_OWNER
     , sch1.SCHEMA_NAME                                -- INDEX_SCHEMA
     , idx.INDEX_NAME                                  -- INDEX_NAME
     , col.COLUMN_NAME                                 -- COLUMN_NAME
     , CAST( ikey.ORDINAL_POSITION AS NUMBER )         -- ORDINAL_POSITION
     , ikey.IS_ASCENDING_ORDER                         -- IS_ASCENDING_ORDER
     , ikey.IS_NULLS_FIRST                             -- IS_NULLS_FIRST
     , CAST( NULL AS NUMBER )                          -- CARDINALITY
     , CAST( NULL AS NUMBER )                          -- PAGES
     , CAST( NULL AS VARCHAR(1024 OCTETS) )            -- FILTER_CONDITION
     , idx.COMMENTS                                    -- COMMENTS 
  FROM
       DEFINITION_SCHEMA.INDEX_KEY_COLUMN_USAGE AS ikey
     , DEFINITION_SCHEMA.INDEXES                AS idx
     , DEFINITION_SCHEMA.COLUMNS                AS col
     , DEFINITION_SCHEMA.TABLES                 AS tab 
     , DEFINITION_SCHEMA.SCHEMATA               AS sch1 
     , DEFINITION_SCHEMA.AUTHORIZATIONS         AS auth1 
     , DEFINITION_SCHEMA.SCHEMATA               AS sch2 
     , DEFINITION_SCHEMA.AUTHORIZATIONS         AS auth2 
 WHERE
       ikey.INDEX_ID          = idx.INDEX_ID
   AND ikey.COLUMN_ID         = col.COLUMN_ID
   AND ikey.TABLE_ID          = tab.TABLE_ID
   AND ikey.INDEX_SCHEMA_ID   = sch1.SCHEMA_ID
   AND ikey.INDEX_OWNER_ID    = auth1.AUTH_ID
   AND ikey.TABLE_SCHEMA_ID   = sch2.SCHEMA_ID
   AND ikey.TABLE_OWNER_ID    = auth2.AUTH_ID
   AND ( tab.TABLE_ID IN ( SELECT pvcol.TABLE_ID 
                             FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES AS pvcol 
                            WHERE ( pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS aucol 
                                                           WHERE aucol.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                        ) 
                                   -- OR  
                                   -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                   --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                  )
                         ) 
         OR 
         tab.TABLE_ID IN ( SELECT pvtab.TABLE_ID 
                             FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES AS pvtab 
                            WHERE ( pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS autab 
                                                           WHERE autab.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                        ) 
                                 -- OR  
                                 -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                 --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                  )
                          ) 
         OR 
         tab.SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID 
                              FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch 
                             WHERE pvsch.PRIVILEGE_TYPE IN ( 'CONTROL SCHEMA', 'ALTER TABLE', 'DROP TABLE', 
                                                             'SELECT TABLE', 'INSERT TABLE', 'DELETE TABLE', 'UPDATE TABLE', 'LOCK TABLE' ) 
                               AND ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch 
                                                            WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                         ) 
                                  -- OR  
                                  -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                   )
                          ) 
         OR 
         EXISTS ( SELECT GRANTEE_ID  
                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba 
                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'ALTER ANY TABLE', 'DROP ANY TABLE', 
                                                   'SELECT ANY TABLE', 'INSERT ANY TABLE', 'DELETE ANY TABLE', 'UPDATE ANY TABLE', 'LOCK ANY TABLE' ) 
                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba 
                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                               ) 
                        -- OR  
                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                         )  
                ) 
       ) 
 ORDER BY 
       ikey.INDEX_SCHEMA_ID
     , ikey.INDEX_ID
     , ikey.ORDINAL_POSITION
) 
UNION ALL
(
SELECT  
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) )  -- TABLE_CATALOG 
     , auth.AUTHORIZATION_NAME                         -- TABLE_OWNER 
     , sch.SCHEMA_NAME                                 -- TABLE_SCHEMA 
     , tab.TABLE_NAME                                  -- TABLE_NAME 
     , CAST( 'TABLE STAT' AS VARCHAR(32 OCTETS) )      -- STAT_TYPE
     , CAST( NULL AS BOOLEAN )                         -- NON_UNIQUE
     , CAST( NULL AS VARCHAR(128 OCTETS) )             -- INDEX_CATALOG
     , CAST( NULL AS VARCHAR(128 OCTETS) )             -- INDEX_OWNER
     , CAST( NULL AS VARCHAR(128 OCTETS) )             -- INDEX_SCHEMA
     , CAST( NULL AS VARCHAR(128 OCTETS) )             -- INDEX_NAME
     , CAST( NULL AS VARCHAR(128 OCTETS) )             -- COLUMN_NAME
     , CAST( NULL AS NUMBER )                          -- ORDINAL_POSITION
     , CAST( NULL AS BOOLEAN )                         -- IS_ASCENDING_ORDER
     , CAST( NULL AS BOOLEAN )                         -- IS_NULLS_FIRST
     , CAST( NULL AS NUMBER )                          -- CARDINALITY
     , CAST( NULL AS NUMBER )                          -- PAGES
     , CAST( NULL AS VARCHAR(1024 OCTETS) )            -- FILTER_CONDITION
     , tab.COMMENTS                                    -- COMMENTS 
  FROM  
       INFORMATION_SCHEMA.WHOLE_TABLES    AS tab 
     , DEFINITION_SCHEMA.SCHEMATA         AS sch 
     , DEFINITION_SCHEMA.AUTHORIZATIONS   AS auth 
 WHERE 
       tab.SCHEMA_ID = sch.SCHEMA_ID 
   AND tab.OWNER_ID  = auth.AUTH_ID 
   AND ( tab.TABLE_ID IN ( SELECT pvcol.TABLE_ID 
                             FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES AS pvcol 
                            WHERE ( pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS aucol 
                                                           WHERE aucol.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                        ) 
                                  -- OR  
                                  -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                  )
                          ) 
         OR 
         tab.TABLE_ID IN ( SELECT pvtab.TABLE_ID 
                             FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES AS pvtab 
                            WHERE ( pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS autab 
                                                           WHERE autab.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                        ) 
                                 -- OR  
                                 -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                 --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                  )
                          ) 
         OR 
         tab.SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID 
                              FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch 
                             WHERE pvsch.PRIVILEGE_TYPE IN ( 'CONTROL SCHEMA', 'ALTER TABLE', 'DROP TABLE', 
                                                             'SELECT TABLE', 'INSERT TABLE', 'DELETE TABLE', 'UPDATE TABLE', 'LOCK TABLE' ) 
                               AND ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch 
                                                            WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                         ) 
                                  -- OR  
                                  -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                   )
                          ) 
         OR 
         EXISTS ( SELECT GRANTEE_ID  
                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba 
                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'ALTER ANY TABLE', 'DROP ANY TABLE', 
                                                   'SELECT ANY TABLE', 'INSERT ANY TABLE', 'DELETE ANY TABLE', 'UPDATE ANY TABLE', 'LOCK ANY TABLE' ) 
                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba 
                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                ) 
                        -- OR  
                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                          )  
                ) 
       ) 
ORDER BY 
      tab.SCHEMA_ID 
    , tab.TABLE_ID 
)
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.STATISTICS 
        IS 'Provides a list of statistics about a single table and the indexes associated with the table that are accessible to a given user or role.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.TABLE_CATALOG                    
        IS 'catalog name of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.TABLE_OWNER                      
        IS 'owner name of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.TABLE_SCHEMA                     
        IS 'schema name of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.TABLE_NAME
        IS 'table name of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.STAT_TYPE
        IS 'statistics type: the value in ( TABLE STAT, INDEX CLUSTERED, INDEX HASHED, INDEX OTHER )';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.NON_UNIQUE
        IS 'indicates whether the index does not allow duplicate values';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.INDEX_CATALOG
        IS 'catalog name of the index';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.INDEX_OWNER
        IS 'owner name of the index';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.INDEX_SCHEMA
        IS 'schema name of the index';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.INDEX_NAME
        IS 'name of the index';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.COLUMN_NAME
        IS 'column name that participates in the index';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.ORDINAL_POSITION
        IS 'ordinal position of the specific column in the index described';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.IS_ASCENDING_ORDER
        IS 'index key column being described is sorted in ASCENDING(TRUE) or DESCENDING(FALSE) order';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.IS_NULLS_FIRST
        IS 'the null values of the key column are sorted before(TRUE) or after(FALSE) non-null values';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.CARDINALITY
        IS 'if STAT_TYPE is (TABLE TYPE), then this is the number of rows in the table; otherwise, it is the number of unique values in the index';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.PAGES
        IS 'if STAT_TYPE is (TABLE TYPE), then this is the number of pages used for the table; otherwise, it is the number of pages used for the current index.';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.FILTER_CONDITION
        IS 'filter condition, if any.';
COMMENT ON COLUMN INFORMATION_SCHEMA.STATISTICS.COMMENTS 
        IS 'if STAT_TYPE is (TABLE TYPE), then this is the table comments; otherwise, it is the index comments.';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.STATISTICS TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.TABLES
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.TABLES;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.TABLES 
( 
       TABLE_CATALOG 
     , TABLE_OWNER 
     , TABLE_SCHEMA 
     , TABLE_NAME 
     , TABLE_TYPE 
     , DBC_TABLE_TYPE 
     , TABLESPACE_NAME 
     , SYSTEM_VERSION_START_COLUMN_NAME 
     , SYSTEM_VERSION_END_COLUMN_NAME 
     , SYSTEM_VERSION_RETENTION_PERIOD 
     , SELF_REFERENCING_COLUMN_NAME 
     , REFERENCE_GENERATION 
     , USER_DEFINED_TYPE_CATALOG 
     , USER_DEFINED_TYPE_SCHEMA 
     , USER_DEFINED_TYPE_NAME 
     , IS_INSERTABLE_INTO 
     , IS_TYPED 
     , COMMIT_ACTION 
     , CREATED_TIME
     , MODIFIED_TIME
     , COMMENTS 
) 
AS 
SELECT  
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth.AUTHORIZATION_NAME 
     , sch.SCHEMA_NAME 
     , tab.TABLE_NAME 
     , tab.TABLE_TYPE 
     , CAST( CASE WHEN sch.SCHEMA_NAME = 'INFORMATION_SCHEMA' AND tab.TABLE_TYPE = 'VIEW'
                  THEN 'SYSTEM TABLE'
                  ELSE DECODE( tab.TABLE_TYPE, 'BASE TABLE', 'TABLE', tab.TABLE_TYPE )
                  END 
             AS VARCHAR(32 OCTETS) )
     , spc.TABLESPACE_NAME 
     , tab.SYSTEM_VERSION_START_COLUMN_NAME 
     , tab.SYSTEM_VERSION_END_COLUMN_NAME 
     , tab.SYSTEM_VERSION_RETENTION_PERIOD 
     , tab.SELF_REFERENCING_COLUMN_NAME 
     , tab.REFERENCE_GENERATION 
     , tab.USER_DEFINED_TYPE_CATALOG 
     , tab.USER_DEFINED_TYPE_SCHEMA 
     , tab.USER_DEFINED_TYPE_NAME 
     , tab.IS_INSERTABLE_INTO 
     , tab.IS_TYPED 
     , tab.COMMIT_ACTION 
     , tab.CREATED_TIME
     , tab.MODIFIED_TIME
     , tab.COMMENTS 
  FROM  
       ( INFORMATION_SCHEMA.WHOLE_TABLES    AS tab 
         LEFT OUTER JOIN 
         DEFINITION_SCHEMA.TABLESPACES      AS spc  
         ON tab.TABLESPACE_ID = spc.TABLESPACE_ID ) 
     , DEFINITION_SCHEMA.SCHEMATA           AS sch 
     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS auth 
 WHERE 
       tab.SCHEMA_ID = sch.SCHEMA_ID 
   AND tab.OWNER_ID  = auth.AUTH_ID 
   AND ( tab.TABLE_ID IN ( SELECT pvcol.TABLE_ID 
                              FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES AS pvcol 
                             WHERE ( pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS aucol 
                                                            WHERE aucol.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                         ) 
                                  -- OR  
                                  -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                   )
                          ) 
         OR 
         tab.TABLE_ID IN ( SELECT pvtab.TABLE_ID 
                             FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES AS pvtab 
                            WHERE ( pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS autab 
                                                           WHERE autab.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                        ) 
                                 -- OR  
                                 -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                 --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                  )
                          ) 
         OR 
         tab.SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID 
                              FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch 
                             WHERE pvsch.PRIVILEGE_TYPE IN ( 'CONTROL SCHEMA', 'ALTER TABLE', 'DROP TABLE', 
                                                             'SELECT TABLE', 'INSERT TABLE', 'DELETE TABLE', 'UPDATE TABLE', 'LOCK TABLE' ) 
                               AND ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch 
                                                            WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                         ) 
                                  -- OR  
                                  -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                   )
                          ) 
         OR 
         EXISTS ( SELECT GRANTEE_ID  
                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba 
                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'ALTER ANY TABLE', 'DROP ANY TABLE', 
                                                   'SELECT ANY TABLE', 'INSERT ANY TABLE', 'DELETE ANY TABLE', 'UPDATE ANY TABLE', 'LOCK ANY TABLE' ) 
                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba 
                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                ) 
                        -- OR  
                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                          )  
                ) 
       ) 
ORDER BY 
      tab.SCHEMA_ID 
    , tab.TABLE_ID 
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.TABLES 
        IS 'Identify the tables defined in this catalog that are accessible to a given user or role';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.TABLE_CATALOG                    
        IS 'catalog name of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.TABLE_OWNER                      
        IS 'owner name of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.TABLE_SCHEMA                     
        IS 'schema name of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.TABLE_NAME                       
        IS 'table name of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.TABLE_TYPE                       
        IS 'the value is in ( BASE TABLE, VIEW, GLOBAL TEMPORARY, LOCAL TEMPORARY, SYSTEM VERSIONED, FIXED TABLE, DUMP TABLE )';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.DBC_TABLE_TYPE                       
        IS 'ODBC/JDBC table type: the value is in ( TABLE, VIEW, GLOBAL TEMPORARY, LOCAL TEMPORARY, SYSTEM TABLE, ALIAS, SYNONYM )';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.TABLESPACE_NAME                  
        IS 'tablespace name of the table, NULL if view';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.SYSTEM_VERSION_START_COLUMN_NAME 
        IS 'if the table is a system-versioned table, then the name of the system-version start column of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.SYSTEM_VERSION_END_COLUMN_NAME   
        IS 'if the table is a system-versioned table, then the name of the system-version end column of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.SYSTEM_VERSION_RETENTION_PERIOD  
        IS 'if the table is a system-versioned table, then the character representation of the value of the retention period of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.SELF_REFERENCING_COLUMN_NAME     
        IS 'if the table is a typed table, then the name of the self-referencing column of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.REFERENCE_GENERATION             
        IS 'if the table has a self-referencing column, the value is in ( SYSTEM GENERATED, USER GENERATED, DERIVED )';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.USER_DEFINED_TYPE_CATALOG        
        IS 'if the table being described is a table of a structured type, the catalog name of the structured type';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.USER_DEFINED_TYPE_SCHEMA         
        IS 'if the table being described is a table of a structured type, the schema name of the structured type';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.USER_DEFINED_TYPE_NAME           
        IS 'if the table being described is a table of a structured type, the name of the structured type';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.IS_INSERTABLE_INTO               
        IS 'is an insertable-into table';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.IS_TYPED                         
        IS 'is a typed table';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.COMMIT_ACTION                    
        IS 'if the table is a temporary table, the value is in ( DELETE, PRESERVE )';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.CREATED_TIME                    
        IS 'created time of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.MODIFIED_TIME                    
        IS 'last modified time of the table';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLES.COMMENTS                         
        IS 'comments of the table';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.TABLES TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.TABLE_CONSTRAINTS
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.TABLE_CONSTRAINTS;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.TABLE_CONSTRAINTS 
(
       CONSTRAINT_CATALOG
     , CONSTRAINT_OWNER
     , CONSTRAINT_SCHEMA
     , CONSTRAINT_NAME
     , TABLE_CATALOG 
     , TABLE_OWNER 
     , TABLE_SCHEMA 
     , TABLE_NAME 
     , CONSTRAINT_TYPE
     , IS_DEFERRABLE
     , INITIALLY_DEFERRED
     , ENFORCED
     , CREATED_TIME
     , MODIFIED_TIME
     , COMMENTS
)
AS
SELECT 
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth1.AUTHORIZATION_NAME
     , sch1.SCHEMA_NAME
     , const.CONSTRAINT_NAME
     , CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth2.AUTHORIZATION_NAME
     , sch2.SCHEMA_NAME
     , tab.TABLE_NAME
     , const.CONSTRAINT_TYPE
     , const.IS_DEFERRABLE
     , const.INITIALLY_DEFERRED
     , const.ENFORCED
     , const.CREATED_TIME
     , const.MODIFIED_TIME
     , const.COMMENTS
  FROM 
       DEFINITION_SCHEMA.TABLE_CONSTRAINTS  AS const
     , DEFINITION_SCHEMA.TABLES             AS tab 
     , DEFINITION_SCHEMA.SCHEMATA           AS sch1 
     , DEFINITION_SCHEMA.SCHEMATA           AS sch2 
     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS auth1 
     , DEFINITION_SCHEMA.AUTHORIZATIONS     AS auth2 
 WHERE 
       const.TABLE_ID             = tab.TABLE_ID
   AND const.CONSTRAINT_SCHEMA_ID = sch1.SCHEMA_ID
   AND const.CONSTRAINT_OWNER_ID  = auth1.AUTH_ID
   AND const.TABLE_SCHEMA_ID      = sch2.SCHEMA_ID
   AND const.TABLE_OWNER_ID       = auth2.AUTH_ID
   AND ( const.TABLE_ID IN ( SELECT pvcol.TABLE_ID 
                               FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES AS pvcol 
                              WHERE pvcol.PRIVILEGE_TYPE <> 'SELECT'
                                AND ( pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                              FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS aucol 
                                                             WHERE aucol.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                          ) 
                                     -- OR  
                                     -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                     --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                    )
                           ) 
         OR 
         const.TABLE_ID IN ( SELECT pvtab.TABLE_ID 
                               FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES AS pvtab 
                              WHERE pvtab.PRIVILEGE_TYPE <> 'SELECT'
                                AND ( pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                              FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS autab 
                                                             WHERE autab.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                          ) 
                                   -- OR  
                                   -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                   --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                    )
                          ) 
         OR 
         const.TABLE_SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID 
                                      FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch 
                                     WHERE pvsch.PRIVILEGE_TYPE IN ( 'CONTROL SCHEMA', 'INSERT TABLE', 'DELETE TABLE', 'UPDATE TABLE' ) 
                                       AND ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                                     FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch 
                                                                    WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                                 ) 
                                          -- OR  
                                          -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                          --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                           )  
                                   ) 
         OR 
         EXISTS ( SELECT GRANTEE_ID  
                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba 
                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'INSERT ANY TABLE', 'DELETE ANY TABLE', 'UPDATE ANY TABLE' ) 
                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba 
                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                ) 
                        -- OR  
                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                          )  
                ) 
       ) 
ORDER BY 
      const.TABLE_SCHEMA_ID 
    , const.TABLE_ID 
    , const.CONSTRAINT_ID
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.TABLE_CONSTRAINTS 
        IS 'Identify the table constraints defined on tables in this catalog that are accessible to a given user or role';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.CONSTRAINT_CATALOG
        IS 'catalog name of the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.CONSTRAINT_OWNER
        IS 'authorization name who owns the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.CONSTRAINT_SCHEMA
        IS 'schema name of the constraint being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.CONSTRAINT_NAME
        IS 'constraint name';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.TABLE_CATALOG
        IS 'catalog name of the table to which the table constraint being described applies';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.TABLE_OWNER 
        IS 'authorization name who owns the table to to which the table constraint being described applies';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.TABLE_SCHEMA
        IS 'schema name of the table to to which the table constraint being described applies';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.TABLE_NAME 
        IS 'table name of the table to to which the table constraint being described applies';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.CONSTRAINT_TYPE
        IS 'the value is in ( PRIMARY KEY, UNIQUE, FOREIGN KEY, NOT NULL, CHECK )';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.IS_DEFERRABLE
        IS 'is a deferrable constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.INITIALLY_DEFERRED
        IS 'is an initially deferred constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.ENFORCED
        IS 'is an enforced constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.CREATED_TIME
        IS 'created time of the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.MODIFIED_TIME
        IS 'last modified time of the constraint';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_CONSTRAINTS.COMMENTS
        IS 'comments of the constraint';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.TABLE_CONSTRAINTS TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.TABLE_PRIVILEGES
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.TABLE_PRIVILEGES;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.TABLE_PRIVILEGES
( 
       GRANTOR
     , GRANTEE
     , TABLE_CATALOG
     , TABLE_OWNER 
     , TABLE_SCHEMA 
     , TABLE_NAME 
     , PRIVILEGE_TYPE
     , IS_GRANTABLE
     , WITH_HIERARCHY
)
AS
SELECT
       grantor.AUTHORIZATION_NAME
     , grantee.AUTHORIZATION_NAME
     , CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , owner.AUTHORIZATION_NAME
     , sch.SCHEMA_NAME
     , tab.TABLE_NAME
     , pvtab.PRIVILEGE_TYPE
     , pvtab.IS_GRANTABLE
     , pvtab.WITH_HIERARCHY
  FROM
       DEFINITION_SCHEMA.TABLE_PRIVILEGES  AS pvtab
     , DEFINITION_SCHEMA.TABLES            AS tab 
     , DEFINITION_SCHEMA.SCHEMATA          AS sch 
     , DEFINITION_SCHEMA.AUTHORIZATIONS    AS grantor
     , DEFINITION_SCHEMA.AUTHORIZATIONS    AS grantee
     , DEFINITION_SCHEMA.AUTHORIZATIONS    AS owner
 WHERE
       pvtab.TABLE_ID   = tab.TABLE_ID
   AND pvtab.SCHEMA_ID  = sch.SCHEMA_ID
   AND pvtab.GRANTOR_ID = grantor.AUTH_ID
   AND pvtab.GRANTEE_ID = grantee.AUTH_ID
   AND tab.OWNER_ID     = owner.AUTH_ID
   AND ( grantee.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )
      -- OR  
      -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
      --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
         OR
         grantor.AUTHORIZATION_NAME = CURRENT_USER
       )
 ORDER BY 
       pvtab.SCHEMA_ID
     , pvtab.TABLE_ID
     , pvtab.GRANTOR_ID
     , pvtab.GRANTEE_ID
     , pvtab.PRIVILEGE_TYPE_ID   
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.TABLE_PRIVILEGES
        IS 'Identify the privileges on tables of tables defined in this catalog that are available to or granted by a given user or role.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_PRIVILEGES.GRANTOR
        IS 'authorization name of the user who granted table privileges';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_PRIVILEGES.GRANTEE
        IS 'authorization name of some user or role, or PUBLIC to indicate all users, to whom the table privilege being described is granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_PRIVILEGES.TABLE_CATALOG
        IS 'catalog name of the table on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_PRIVILEGES.TABLE_OWNER 
        IS 'table owner name of the table on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_PRIVILEGES.TABLE_SCHEMA 
        IS 'schema name of the table on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_PRIVILEGES.TABLE_NAME 
        IS 'table name on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_PRIVILEGES.PRIVILEGE_TYPE
        IS 'the value is in ( CONTROL, SELECT, INSERT, UPDATE, DELETE, REFERENCES, LOCK, INDEX, ALTER )';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_PRIVILEGES.IS_GRANTABLE
        IS 'is grantable';
COMMENT ON COLUMN INFORMATION_SCHEMA.TABLE_PRIVILEGES.WITH_HIERARCHY
        IS 'whether the privilege was granted WITH HIERARCHY OPTION or not';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.TABLE_PRIVILEGES TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.USAGE_PRIVILEGES
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.USAGE_PRIVILEGES;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.USAGE_PRIVILEGES
( 
       GRANTOR
     , GRANTEE
     , OBJECT_CATALOG
     , OBJECT_OWNER 
     , OBJECT_SCHEMA 
     , OBJECT_NAME 
     , OBJECT_TYPE
     , PRIVILEGE_TYPE
     , IS_GRANTABLE
)
AS
SELECT
       grantor.AUTHORIZATION_NAME
     , grantee.AUTHORIZATION_NAME
     , CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , owner.AUTHORIZATION_NAME
     , sch.SCHEMA_NAME
     , sqc.SEQUENCE_NAME
     , usg.OBJECT_TYPE
     , CAST( 'USAGE' AS VARCHAR(32 OCTETS) )
     , usg.IS_GRANTABLE
  FROM
       DEFINITION_SCHEMA.USAGE_PRIVILEGES  AS usg
     , DEFINITION_SCHEMA.SEQUENCES         AS sqc
     , DEFINITION_SCHEMA.SCHEMATA          AS sch
     , DEFINITION_SCHEMA.AUTHORIZATIONS    AS grantor
     , DEFINITION_SCHEMA.AUTHORIZATIONS    AS grantee
     , DEFINITION_SCHEMA.AUTHORIZATIONS    AS owner
 WHERE
       usg.OBJECT_ID    = sqc.SEQUENCE_ID
   AND usg.SCHEMA_ID    = sch.SCHEMA_ID
   AND usg.GRANTOR_ID   = grantor.AUTH_ID
   AND usg.GRANTEE_ID   = grantee.AUTH_ID
   AND sqc.OWNER_ID     = owner.AUTH_ID
   AND ( grantee.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )
      -- OR  
      -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
      --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
         OR
         grantor.AUTHORIZATION_NAME = CURRENT_USER
       )
 ORDER BY 
       usg.SCHEMA_ID
     , usg.OBJECT_ID
     , usg.GRANTOR_ID
     , usg.GRANTEE_ID
;

--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.USAGE_PRIVILEGES
        IS 'Identify the USAGE privileges on objects defined in this catalog that are available to or granted by a given user or role.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.USAGE_PRIVILEGES.GRANTOR
        IS 'authorization name of the user who granted usage privileges, on the object of the type identified by OBJECT_TYPE';
COMMENT ON COLUMN INFORMATION_SCHEMA.USAGE_PRIVILEGES.GRANTEE
        IS 'authorization identifier of some user or role, or PUBLIC to indicate all users, to whom the usage privilege being described is granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.USAGE_PRIVILEGES.OBJECT_CATALOG
        IS 'catalog name of the object of the type identified by OBJECT_TYPE on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.USAGE_PRIVILEGES.OBJECT_OWNER 
        IS 'owner name of the object of the type identified by OBJECT_TYPE on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.USAGE_PRIVILEGES.OBJECT_SCHEMA 
        IS 'schema name of the object of the type identified by OBJECT_TYPE on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.USAGE_PRIVILEGES.OBJECT_NAME 
        IS 'object name of the type identified by OBJECT_TYPE on which the privilege being described was granted';
COMMENT ON COLUMN INFORMATION_SCHEMA.USAGE_PRIVILEGES.OBJECT_TYPE 
        IS 'the value is in ( DOMAIN, CHARACTER SET, COLLATION, TRANSLATION, SEQUENCE )';
COMMENT ON COLUMN INFORMATION_SCHEMA.USAGE_PRIVILEGES.PRIVILEGE_TYPE
        IS 'the value is in ( USAGE )';
COMMENT ON COLUMN INFORMATION_SCHEMA.USAGE_PRIVILEGES.IS_GRANTABLE
        IS 'is grantable';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.USAGE_PRIVILEGES TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.VIEWS
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.VIEWS;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.VIEWS
(
       TABLE_CATALOG
     , TABLE_OWNER
     , TABLE_SCHEMA
     , TABLE_NAME
     , VIEW_DEFINITION
     , CHECK_OPTION
     , IS_UPDATABLE
     , INSERTABLE_INTO
     , IS_TRIGGER_UPDATABLE
     , IS_TRIGGER_DELETABLE
     , IS_TRIGGER_INSERTABLE_INTO
     , IS_COMPILED
     , IS_AFFECTED
     , COMMENTS
)
AS
SELECT
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth1.AUTHORIZATION_NAME
     , sch.SCHEMA_NAME
     , tab.TABLE_NAME
     , CASE WHEN ( CURRENT_USER IN ( auth1.AUTHORIZATION_NAME, auth2.AUTHORIZATION_NAME ) ) THEN viw.VIEW_DEFINITION ELSE CAST( 'not owner' AS LONG VARCHAR ) END
     , viw.CHECK_OPTION
     , viw.IS_UPDATABLE
     , tab.IS_INSERTABLE_INTO
     , viw.IS_TRIGGER_UPDATABLE
     , viw.IS_TRIGGER_DELETABLE
     , viw.IS_TRIGGER_INSERTABLE_INTO
     , viw.IS_COMPILED
     , viw.IS_AFFECTED
     , tab.COMMENTS
  FROM 
       DEFINITION_SCHEMA.VIEWS            AS viw
     , DEFINITION_SCHEMA.TABLES           AS tab 
     , DEFINITION_SCHEMA.SCHEMATA         AS sch 
     , DEFINITION_SCHEMA.AUTHORIZATIONS   AS auth1 
     , DEFINITION_SCHEMA.AUTHORIZATIONS   AS auth2 
 WHERE 
       viw.TABLE_ID  = tab.TABLE_ID
   AND viw.SCHEMA_ID = sch.SCHEMA_ID 
   AND viw.OWNER_ID  = auth1.AUTH_ID 
   AND sch.OWNER_ID  = auth2.AUTH_ID
   AND ( viw.TABLE_ID IN ( SELECT pvcol.TABLE_ID 
                             FROM DEFINITION_SCHEMA.COLUMN_PRIVILEGES AS pvcol 
                            WHERE ( pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS aucol 
                                                           WHERE aucol.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                        ) 
                                 -- OR  
                                 -- pvcol.GRANTEE_ID IN ( SELECT AUTH_ID 
                                 --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                  )
                         ) 
         OR 
         viw.TABLE_ID IN ( SELECT pvtab.TABLE_ID 
                             FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES AS pvtab 
                            WHERE ( pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                            FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS autab 
                                                           WHERE autab.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                        ) 
                                 -- OR  
                                 -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                 --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                  )
                          ) 
         OR 
         viw.SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID 
                              FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch 
                             WHERE pvsch.PRIVILEGE_TYPE IN ( 'CONTROL SCHEMA', 'DROP VIEW', 
                                                             'SELECT TABLE', 'INSERT TABLE', 'DELETE TABLE', 'UPDATE TABLE', 'LOCK TABLE' ) 
                               AND ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                             FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch 
                                                            WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                         ) 
                                  -- OR  
                                  -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                  --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                   )
                          ) 
         OR 
         EXISTS ( SELECT GRANTEE_ID  
                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba 
                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'ALTER ANY TABLE', 'DROP ANY VIEW', 
                                                   'SELECT ANY TABLE', 'INSERT ANY TABLE', 'DELETE ANY TABLE', 'UPDATE ANY TABLE', 'LOCK ANY TABLE' ) 
                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba 
                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                ) 
                        -- OR  
                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                          )  
                ) 
       ) 
ORDER BY 
      viw.SCHEMA_ID 
    , viw.TABLE_ID 
;



--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.VIEWS
        IS 'Identify the viewed tables defined in this catalog that are accessible to a given user or role.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.TABLE_CATALOG
        IS 'catalog name of the viewed table';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.TABLE_OWNER
        IS 'owner name of the viewed table';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.TABLE_SCHEMA
        IS 'schema name of the viewed table';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.TABLE_NAME
        IS 'view name of the viewed table';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.VIEW_DEFINITION
        IS 'the character representation of the user-specified query expression contained in the corresponding view descriptor';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.CHECK_OPTION
        IS 'the value is in ( CASCADED, LOCAL, NONE )';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.IS_UPDATABLE
        IS 'is an updatable view';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.INSERTABLE_INTO
        IS 'is an insertable view';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.IS_TRIGGER_UPDATABLE
        IS 'whether an update INSTEAD OF trigger is defined on the view or not';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.IS_TRIGGER_DELETABLE
        IS 'whether a delete INSTEAD OF trigger is defined on the view or not';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.IS_TRIGGER_INSERTABLE_INTO
        IS 'whether an insert INSTEAD OF trigger is defined on the view or not';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.IS_COMPILED
        IS 'whether the view is compiled or not';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.IS_AFFECTED
        IS 'whether the view is affected by modification of underlying object or not';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEWS.COMMENTS
        IS 'comments of the view';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.VIEWS TO PUBLIC;

COMMIT;

--##############################################################
--# INFORMATION_SCHEMA.VIEW_TABLE_USAGE
--##############################################################

--#####################
--# drop view
--#####################

DROP VIEW IF EXISTS INFORMATION_SCHEMA.VIEW_TABLE_USAGE;

--#####################
--# create view
--#####################

CREATE VIEW INFORMATION_SCHEMA.VIEW_TABLE_USAGE
(
       VIEW_CATALOG
     , VIEW_OWNER
     , VIEW_SCHEMA
     , VIEW_NAME
     , TABLE_CATALOG
     , TABLE_OWNER
     , TABLE_SCHEMA
     , TABLE_NAME
)
AS
SELECT 
       CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth1.AUTHORIZATION_NAME
     , sch1.SCHEMA_NAME
     , tab1.TABLE_NAME
     , CAST( CURRENT_CATALOG AS VARCHAR(128 OCTETS) ) 
     , auth2.AUTHORIZATION_NAME
     , sch2.SCHEMA_NAME
     , tab2.TABLE_NAME
  FROM 
       DEFINITION_SCHEMA.VIEW_TABLE_USAGE AS vtu
     , INFORMATION_SCHEMA.WHOLE_TABLES    AS tab1 
     , INFORMATION_SCHEMA.WHOLE_TABLES    AS tab2 
     , DEFINITION_SCHEMA.SCHEMATA         AS sch1 
     , DEFINITION_SCHEMA.SCHEMATA         AS sch2 
     , DEFINITION_SCHEMA.AUTHORIZATIONS   AS auth1 
     , DEFINITION_SCHEMA.AUTHORIZATIONS   AS auth2 
 WHERE 
       vtu.VIEW_ID         = tab1.TABLE_ID
   AND vtu.VIEW_SCHEMA_ID  = sch1.SCHEMA_ID 
   AND vtu.VIEW_OWNER_ID   = auth1.AUTH_ID 
   AND vtu.TABLE_ID        = tab2.TABLE_ID
   AND vtu.TABLE_SCHEMA_ID = sch2.SCHEMA_ID 
   AND vtu.TABLE_OWNER_ID  = auth2.AUTH_ID 
   AND ( vtu.VIEW_ID IN ( SELECT pvtab.TABLE_ID 
                            FROM DEFINITION_SCHEMA.TABLE_PRIVILEGES AS pvtab 
                           WHERE ( pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                           FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS autab 
                                                          WHERE autab.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                       ) 
                                -- OR  
                                -- pvtab.GRANTEE_ID IN ( SELECT AUTH_ID 
                                --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                 )
                         ) 
         OR 
         vtu.VIEW_SCHEMA_ID IN ( SELECT pvsch.SCHEMA_ID 
                                   FROM DEFINITION_SCHEMA.SCHEMA_PRIVILEGES AS pvsch 
                                  WHERE pvsch.PRIVILEGE_TYPE IN ( 'CONTROL SCHEMA', 'DROP VIEW', 
                                                                  'SELECT TABLE', 'INSERT TABLE', 'DELETE TABLE', 'UPDATE TABLE', 'LOCK TABLE' ) 
                                    AND ( pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                                  FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS ausch 
                                                                 WHERE ausch.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                              ) 
                                       -- OR  
                                       -- pvsch.GRANTEE_ID IN ( SELECT AUTH_ID 
                                       --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                                        )
                               ) 
         OR 
         EXISTS ( SELECT GRANTEE_ID  
                    FROM DEFINITION_SCHEMA.DATABASE_PRIVILEGES pvdba 
                   WHERE pvdba.PRIVILEGE_TYPE IN ( 'ALTER ANY TABLE', 'DROP ANY VIEW', 
                                                   'SELECT ANY TABLE', 'INSERT ANY TABLE', 'DELETE ANY TABLE', 'UPDATE ANY TABLE', 'LOCK ANY TABLE' ) 
                     AND ( pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                                                   FROM DEFINITION_SCHEMA.AUTHORIZATIONS AS audba 
                                                  WHERE audba.AUTHORIZATION_NAME IN ( 'PUBLIC', CURRENT_USER )  
                                                ) 
                        -- OR  
                        -- pvdba.GRANTEE_ID IN ( SELECT AUTH_ID 
                        --                         FROM INORMATION_SCHEMA.ENABLED_ROLES )  
                          )  
                ) 
       ) 
ORDER BY 
      vtu.VIEW_SCHEMA_ID 
    , vtu.VIEW_ID 
    , vtu.TABLE_SCHEMA_ID
    , vtu.TABLE_ID
;



--#####################
--# comment view
--#####################

COMMENT ON TABLE INFORMATION_SCHEMA.VIEW_TABLE_USAGE
        IS 'Identify the tables on which viewed tables defined in this catalog and owned by a given user or role are dependent.';

--#####################
--# comment column
--#####################

COMMENT ON COLUMN INFORMATION_SCHEMA.VIEW_TABLE_USAGE.VIEW_CATALOG
        IS 'catalog name of the viewed table';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEW_TABLE_USAGE.VIEW_OWNER
        IS 'owner name of the viewed table';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEW_TABLE_USAGE.VIEW_SCHEMA
        IS 'schema name of the viewed table';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEW_TABLE_USAGE.VIEW_NAME
        IS 'view name of the viewed table';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEW_TABLE_USAGE.TABLE_CATALOG
        IS 'catalog name of a table that is explicitly or implicitly referenced in the original query expression of the compiled view being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEW_TABLE_USAGE.TABLE_OWNER
        IS 'owner name of a table that is explicitly or implicitly referenced in the original query expression of the compiled view being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEW_TABLE_USAGE.TABLE_SCHEMA
        IS 'schema name of a table that is explicitly or implicitly referenced in the original query expression of the compiled view being described';
COMMENT ON COLUMN INFORMATION_SCHEMA.VIEW_TABLE_USAGE.TABLE_NAME
        IS 'table name of a table that is explicitly or implicitly referenced in the original query expression of the compiled view being described';

--#####################
--# grant view
--#####################

GRANT SELECT ON TABLE INFORMATION_SCHEMA.VIEW_TABLE_USAGE TO PUBLIC;

COMMIT;



