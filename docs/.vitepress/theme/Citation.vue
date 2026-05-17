<script setup lang="ts">
/**
 * Citation.vue - Academic citation system component
 *
 * Usage:
 * <Citation :references="[
 *   { id: 'drepper2007', author: 'Drepper, U.', title: 'What Every Programmer Should Know About Memory', year: 2007, url: '...' }
 * ]" />
 */

interface Reference {
  id: string
  author: string
  title: string
  year?: number
  source?: string
  url?: string
  doi?: string
}

interface Props {
  references: Reference[]
  title?: string
  numbered?: boolean
}

const props = withDefaults(defineProps<Props>(), {
  title: 'References',
  numbered: true,
})

function formatCitation(ref: Reference): string {
  const parts = [ref.author]
  if (ref.year) parts.push(`(${ref.year})`)
  parts.push(`*${ref.title}*`)
  if (ref.source) parts.push(ref.source)
  return parts.join('. ')
}
</script>

<template>
  <section class="wp-citations">
    <h3>{{ title }}</h3>
    <ol v-if="numbered" class="wp-citations-list">
      <li v-for="ref in references" :key="ref.id" class="wp-citation-item">
        <span class="citation-author">{{ ref.author }}</span>
        <span v-if="ref.year" class="citation-year"> ({{ ref.year }})</span>.
        <span class="citation-title">{{ ref.title }}</span>
        <span v-if="ref.source" class="citation-source">. {{ ref.source }}</span>
        <span v-if="ref.url || ref.doi" class="citation-link">
          <a :href="ref.url || `https://doi.org/${ref.doi}`" target="_blank" rel="noopener noreferrer">
            {{ ref.doi ? `DOI: ${ref.doi}` : 'Link' }}
          </a>
        </span>
      </li>
    </ol>
    <ul v-else class="wp-citations-list">
      <li v-for="ref in references" :key="ref.id" class="wp-citation-item">
        <span class="citation-author">{{ ref.author }}</span>
        <span v-if="ref.year" class="citation-year"> ({{ ref.year }})</span>.
        <span class="citation-title">{{ ref.title }}</span>
        <span v-if="ref.source" class="citation-source">. {{ ref.source }}</span>
        <span v-if="ref.url || ref.doi" class="citation-link">
          <a :href="ref.url || `https://doi.org/${ref.doi}`" target="_blank" rel="noopener noreferrer">
            {{ ref.doi ? `DOI: ${ref.doi}` : 'Link' }}
          </a>
        </span>
      </li>
    </ul>
  </section>
</template>

<style scoped>
.wp-citations {
  margin: var(--wp-space-7, 3rem) 0;
  padding: var(--wp-space-5, 1.5rem);
  border: 1px solid var(--wp-line-1, #e0e0e0);
  border-radius: var(--wp-radius-2, 14px);
  background: var(--wp-surface-1, #f8f9fa);
}

.wp-citations h3 {
  margin-top: 0;
  margin-bottom: var(--wp-space-4, 1rem);
  color: var(--wp-ink-1, #1a1a2e);
  font-size: 1.1rem;
  font-weight: 700;
  letter-spacing: -0.01em;
}

.wp-citations-list {
  margin: 0;
  padding-left: var(--wp-space-5, 1.5rem);
}

.wp-citation-item {
  margin-bottom: var(--wp-space-3, 0.75rem);
  color: var(--wp-ink-2, #4a4a6a);
  font-size: 0.95rem;
  line-height: 1.7;
}

.wp-citation-item:last-child {
  margin-bottom: 0;
}

.citation-author {
  font-weight: 600;
  color: var(--wp-ink-1, #1a1a2e);
}

.citation-year {
  color: var(--wp-ink-3, #6a6a8a);
}

.citation-title {
  font-style: italic;
}

.citation-source {
  color: var(--wp-ink-3, #6a6a8a);
}

.citation-link {
  margin-left: 0.5rem;
}

.citation-link a {
  color: var(--wp-accent-primary, #3b6bdc);
  font-size: 0.85rem;
  text-decoration: none;
  transition: color 0.15s ease;
}

.citation-link a:hover {
  color: var(--wp-accent-secondary, #5a8bfc);
  text-decoration: underline;
}
</style>
