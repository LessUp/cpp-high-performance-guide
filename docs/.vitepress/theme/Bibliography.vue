<script setup lang="ts">
import { computed } from 'vue'

interface Citation {
  id: string
  type: 'article' | 'book' | 'conference' | 'online'
  authors: string[]
  title: string
  year: number
  source?: string
  publisher?: string
  booktitle?: string
  volume?: string
  number?: string
  pages?: string
  doi?: string
  url?: string
  isbn?: string
  edition?: string
  accessed?: string
  tags?: string[]
}

const props = defineProps<{
  citations: Citation[]
  style?: 'apa' | 'ieee' | 'acm'
  title?: string
}>()

const sortedCitations = computed(() =>
  [...props.citations].sort((a, b) => a.id.localeCompare(b.id))
)

const citationStyle = computed(() => props.style || 'apa')

function formatAPA(citation: Citation): string {
  const authors = citation.authors.length > 2
    ? `${citation.authors[0]} et al.`
    : citation.authors.join(' & ')

  let result = `${authors} (${citation.year}). ${citation.title}.`

  if (citation.type === 'article' && citation.source) {
    result += ` *${citation.source}*`
  } else if (citation.type === 'book' && citation.publisher) {
    result += ` ${citation.publisher}`
  } else if (citation.type === 'conference' && citation.booktitle) {
    result += ` In *${citation.booktitle}*`
    if (citation.pages) result += ` (pp. ${citation.pages})`
  }

  return result
}

function formatIEEE(citation: Citation): string {
  const authors = citation.authors.join(', ')

  let result = `${authors}, "${citation.title},"`

  if (citation.type === 'book') {
    if (citation.edition) result += ` ${citation.edition} ed.`
    if (citation.publisher) result += ` ${citation.publisher},`
  } else if (citation.type === 'conference' && citation.booktitle) {
    result += ` in *${citation.booktitle}*`
    if (citation.pages) result += `, pp. ${citation.pages},`
  } else if (citation.type === 'article' && citation.source) {
    result += ` *${citation.source}*`
    if (citation.volume) result += `, vol. ${citation.volume}`
    if (citation.number) result += `, no. ${citation.number}`
  }

  result += ` ${citation.year}.`

  return result
}

function formatCitation(citation: Citation): string {
  switch (citationStyle.value) {
    case 'ieee':
      return formatIEEE(citation)
    case 'apa':
    default:
      return formatAPA(citation)
  }
}

function generateBibTeX(citation: Citation): string {
  const typeMap: Record<string, string> = {
    article: 'article',
    book: 'book',
    conference: 'inproceedings',
    online: 'misc'
  }
  const bibtexType = typeMap[citation.type] || 'misc'

  let bibtex = `@${bibtexType}{${citation.id},\n`
  bibtex += `  author = {${citation.authors.join(' and ')}},\n`
  bibtex += `  title = {${citation.title}},\n`
  bibtex += `  year = {${citation.year}},\n`

  if (citation.publisher) bibtex += `  publisher = {${citation.publisher}},\n`
  if (citation.booktitle) bibtex += `  booktitle = {${citation.booktitle}},\n`
  if (citation.doi) bibtex += `  doi = {${citation.doi}},\n`
  if (citation.url) bibtex += `  url = {${citation.url}},\n`
  if (citation.isbn) bibtex += `  isbn = {${citation.isbn}},\n`
  if (citation.pages) bibtex += `  pages = {${citation.pages}},\n`

  bibtex += '}'
  return bibtex
}

function copyBibTeX(citation: Citation) {
  const bibtex = generateBibTeX(citation)
  navigator.clipboard.writeText(bibtex)
}
</script>

<template>
  <section class="wp-bibliography">
    <h2 v-if="title">{{ title }}</h2>
    <ol class="wp-bibliography-list">
      <li
        v-for="citation in sortedCitations"
        :key="citation.id"
        class="wp-bibliography-item"
      >
        <span class="wp-bibliography-id">[{{ citation.id }}]</span>
        <span class="wp-bibliography-text">
          {{ formatCitation(citation) }}
        </span>
        <div class="wp-bibliography-actions">
          <a
            v-if="citation.doi"
            :href="`https://doi.org/${citation.doi}`"
            target="_blank"
            rel="noopener"
            class="wp-bibliography-link"
          >
            DOI
          </a>
          <a
            v-if="citation.url"
            :href="citation.url"
            target="_blank"
            rel="noopener"
            class="wp-bibliography-link"
          >
            Link
          </a>
          <button
            class="wp-bibliography-bibtex"
            @click="copyBibTeX(citation)"
            title="Copy BibTeX citation"
          >
            BibTeX
          </button>
        </div>
      </li>
    </ol>
  </section>
</template>

<style scoped>
.wp-bibliography {
  margin: var(--wp-space-7) 0;
  padding: var(--wp-space-5);
  border: 1px solid var(--wp-line-1);
  border-radius: var(--wp-radius-2);
  background: var(--wp-surface-1);
}

.wp-bibliography h2 {
  margin-top: 0;
  margin-bottom: var(--wp-space-4);
  color: var(--wp-ink-1);
  font-size: 1.2rem;
  font-weight: 700;
}

.wp-bibliography-list {
  margin: 0;
  padding-left: var(--wp-space-5);
}

.wp-bibliography-item {
  margin-bottom: var(--wp-space-4);
  color: var(--wp-ink-2);
  font-size: 0.95rem;
  line-height: 1.7;
}

.wp-bibliography-item:last-child {
  margin-bottom: 0;
}

.wp-bibliography-id {
  font-family: var(--wp-font-mono);
  font-size: 0.85em;
  color: var(--wp-accent-primary);
  margin-right: var(--wp-space-2);
}

.wp-bibliography-text {
  color: var(--wp-ink-2);
}

.wp-bibliography-actions {
  display: inline-flex;
  gap: var(--wp-space-2);
  margin-left: var(--wp-space-3);
}

.wp-bibliography-link {
  padding: 0.15rem 0.4rem;
  border-radius: var(--wp-radius-1);
  background: var(--wp-accent-soft);
  color: var(--wp-accent-primary);
  font-size: 0.75rem;
  font-weight: 600;
  text-decoration: none;
  transition: all var(--wp-transition-fast);
}

.wp-bibliography-link:hover {
  background: var(--wp-accent-primary);
  color: white;
}

.wp-bibliography-bibtex {
  padding: 0.15rem 0.4rem;
  border: 1px solid var(--wp-line-1);
  border-radius: var(--wp-radius-1);
  background: var(--wp-surface-2);
  color: var(--wp-ink-3);
  font-size: 0.75rem;
  font-weight: 600;
  cursor: pointer;
  transition: all var(--wp-transition-fast);
}

.wp-bibliography-bibtex:hover {
  border-color: var(--wp-accent-primary);
  color: var(--wp-accent-primary);
}
</style>
